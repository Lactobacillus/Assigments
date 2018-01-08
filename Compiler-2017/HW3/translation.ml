(*
* Homwork #3 - translation.ml
* Compiler 2017
* Author : Cho Youngwoo
* 2017.05.25
*)

open While
open M

let rec trans_aexp : aexp -> inst list
= fun e ->
  match e with
  | NUM i -> [Push i]
  | VAR x -> [Fetch x]
  | ADD (a1, a2) -> (trans_aexp a2)@(trans_aexp a1)@[Add]
  | SUB (a1, a2) -> (trans_aexp a2)@(trans_aexp a1)@[Sub]
  | MUL (a1, a2) -> (trans_aexp a2)@(trans_aexp a1)@[Mul]

let rec trans_bexp : bexp -> inst list
= fun e ->
  match e with
  | TRUE -> [True]
  | FALSE -> [False]
  | EQ (a1, a2) -> (trans_aexp a2)@(trans_aexp a1)@[Eq]
  | LE (a1, a2) -> (trans_aexp a2)@(trans_aexp a1)@[Le]
  | NEG b -> (trans_bexp b)@[Neg]
  | AND (b1, b2) -> (trans_bexp b2)@(trans_bexp b1)@[And]

let rec trans : stmt -> inst list
= fun c ->
  match c with
  | ASSIGN (x, a) -> (trans_aexp a)@[Store x]
  | SKIP -> [Noop]
  | SEQ (c1, c2) -> (trans c1)@(trans c2)
  | IF (b, c1, c2) -> (trans_bexp b)@[Branch (trans c1, trans c2)]
  | WHILE (b, c1) -> (trans_bexp b)@[Loop (trans_bexp b, trans c1)]
  | READ x -> [Read]@[Store x]
  | PRINT e -> (trans_aexp e)@[Print]

