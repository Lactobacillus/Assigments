(*
* Homwork #3 - m.ml
* Compiler 2017
* Author : Cho Youngwoo
* 2017.05.25
*)

(**************************)
(*    syntax              *)
(**************************)
type inst = 
  | Push of int 
  | Add
  | Mul
  | Sub
  | True
  | False
  | Eq
  | Le
  | And
  | Neg
  | Fetch of var
  | Store of var 
  | Noop
  | Branch of cmd * cmd
  | Loop of cmd * cmd
  | Read
  | Print

and cmd = inst list
and var = string

(**************************)
(*    pretty printer      *)
(**************************)
let rec i2s inst =
  match inst with
  | Push n -> "push("^string_of_int n^")"
  | Add -> "add"
  | Mul -> "mul"
  | Sub -> "sub"
  | True -> "true"
  | False -> "false"
  | Eq -> "eq"
  | Le -> "le"
  | And -> "and"
  | Neg -> "neg"
  | Fetch x -> "fetch("^x^")"
  | Store x -> "store("^x^")"
  | Noop -> "noop"
  | Branch (c1,c2) -> "branch("^c2s c1^","^c2s c2^")"
  | Loop (c1,c2) -> "loop("^c2s c1^","^c2s c2^")"
  | Read -> "read"
  | Print -> "print"
and c2s cmd = List.fold_left (fun s i -> s ^ " " ^ i2s i) "" cmd
let pp cmd = print_endline (c2s cmd)

(**************************)
(*    semantics           *)
(**************************)
type value = Z of int | T of bool
type stack = value list
type state = (var, int) PMap.t

let state_empty = PMap.empty
let state_lookup x s = PMap.find x s
let state_bind x v s = PMap.add x v s

exception Wrong_instruction

let next : inst list * stack * state -> inst list * stack * state
= fun (c,e,s) -> 
  match (c, e, s) with
  | [], e, s -> ([], e, s)
  | inst::l, e, s ->
    match inst with
    | Push n -> (l, (Z n)::e, s)
    | Add ->
      (match e with
      | z1::z2::ee ->
        (match z1, z2 with
        | Z n1, Z n2 -> (l, (Z (n1 + n2))::ee, s)
        | _ -> raise Wrong_instruction)
      | _ -> raise Wrong_instruction)
    | Mul ->
      (match e with
      | z1::z2::ee ->
        (match z1, z2 with
        | Z n1, Z n2 -> (l, (Z (n1 * n2))::ee, s)
        | _ -> raise Wrong_instruction)
      | _ -> raise Wrong_instruction)
    | Sub ->
      (match e with
      | z1::z2::ee ->
        (match z1, z2 with
        | Z n1, Z n2 -> (l, (Z (n1 - n2))::ee, s)
        | _ -> raise Wrong_instruction)
      | _ -> raise Wrong_instruction)
    | True -> (l, (T true)::e, s)
    | False -> (l, (T false)::e, s)
    | Eq ->
      (match e with
      | z1::z2::ee -> if z1 = z2 then (l, (T true)::ee, s) else (l, (T false)::ee, s)
      | _ -> raise Wrong_instruction)
    | Le ->
      (match e with
      | z1::z2::ee -> if z1 <= z2 then (l, (T true)::ee, s) else (l, (T false)::ee, s)
      | _ -> raise Wrong_instruction)
    | And ->
      (match e with
      | t1::t2::ee -> if t1 = (T true) && t2 = (T true) then (l, (T true)::ee, s) else (l, (T false)::ee, s)
      | _ -> raise Wrong_instruction)
    | Neg ->
      (match e with
      | t::ee -> if t = (T false) then (l, (T true)::ee, s) else (l, (T false)::ee, s)
      | _ -> raise Wrong_instruction)
    | Fetch x -> (l, (Z (state_lookup x s))::e, s)
    | Store x ->
      (match e with
      | Z z::ee -> (l, ee, (state_bind x z s))
      | _ -> raise Wrong_instruction)
    | Noop -> (l, e, s)
    | Branch (c1, c2) ->
      (match e with
      | T t::ee -> if t = true then (c1@l, e, s) else (c2@l, e, s)
      | _ -> raise Wrong_instruction)
    | Loop (c1, c2) -> (c1@[Branch(c2@[Loop(c1, c2)], [Noop])]@l, e, s) 
    | Read -> (l, (Z (read_int ()))::e, s)
    | Print ->
      (match e with
      | Z n::ee -> print_endline (string_of_int n); (l, e, s)
      | _ -> raise (Failure "print: not an integer"))

let run : cmd -> state
= fun c -> 
  let iconf = (c, [], state_empty) in
  let rec iter (c,e,s) = 
    match next (c,e,s) with
    | [], _, s  -> s
    | c', e',s' -> iter (c',e',s') in
    iter iconf
