(* translator.ml
 * Compiler 2017
 * Author : Cho Youngwoo
 * 2017.06.14
*)

open T
open S

let label = ref 0
let variable_string = "뚜루뚜뚜따라따따"
let variable_count = ref 0
(* !label 을 하면 값 참조 *)
(* label := 100 을 하면 100 저장 *)

let make_variable : unit -> T.var
= fun u ->
  let _ = (variable_count := !variable_count + 1) in
    variable_string^(string_of_int !variable_count)

let rec translate_exp : S.exp -> T.var * (T.linstr list)
= fun e -> begin match e with
  | NUM n -> begin
    let v = make_variable () in
    let c = [(0, COPYC (v, n))] in
      (v, c)
    end
  | LV lv -> begin match lv with
    | ID x -> begin
      let v = make_variable () in
      let c = [(0, COPY (v, x))] in
        (v, c)
      end
    | ARR (x, e) -> begin
      let v = make_variable () in
      let (v1, c1) = translate_exp e in
      let c = c1@[(0, LOAD (v, (x, v1)))] in
        (v, c)
      end
    end
  | ADD (e1, e2) -> begin
    let v = make_variable () in
    let (v1, c1) = translate_exp e1 in
    let (v2, c2) = translate_exp e2 in
    let c = c1@c2@[(0, ASSIGNV (v ,ADD, v1, v2))] in
      (v, c)
    end
  | SUB (e1, e2) -> begin
    let v = make_variable () in
    let (v1, c1) = translate_exp e1 in
    let (v2, c2) = translate_exp e2 in
    let c = c1@c2@[(0, ASSIGNV (v, SUB, v1, v2))] in
      (v, c)
    end
  | MUL (e1, e2) -> begin
    let v = make_variable () in
    let (v1, c1) = translate_exp e1 in
    let (v2, c2) = translate_exp e2 in
    let c = c1@c2@[(0, ASSIGNV (v, MUL, v1, v2))] in
      (v, c)
    end
  | DIV (e1, e2) -> begin
    let v = make_variable () in
    let (v1, c1) = translate_exp e1 in
    let (v2, c2) = translate_exp e2 in
    let c = c1@c2@[(0, ASSIGNV (v, DIV, v1, v2))] in
      (v, c)
    end
  | MINUS e -> begin
    let v = make_variable () in
    let (v1, c1) = translate_exp e in
    let c = c1@[(0, ASSIGNU (v, MINUS, v1))] in
      (v, c)
    end
  | NOT e -> begin
    let v = make_variable () in
    let (v1, c1) = translate_exp e in
    let c = c1@[(0, ASSIGNU (v, NOT, v1))] in
      (v, c)
    end
  | LT (e1, e2) -> begin
    let v = make_variable () in
    let (v1, c1) = translate_exp e1 in
    let (v2, c2) = translate_exp e2 in
    let c = c1@c2@[(0, ASSIGNV (v, LT, v1, v2))] in
      (v, c)
    end
  | LE (e1, e2) -> begin
    let v = make_variable () in
    let (v1, c1) = translate_exp e1 in
    let (v2, c2) = translate_exp e2 in
    let c = c1@c2@[(0, ASSIGNV (v, LE, v1, v2))] in
      (v, c)
    end
  | GT (e1, e2) -> begin
    let v = make_variable () in
    let (v1, c1) = translate_exp e1 in
    let (v2, c2) = translate_exp e2 in
    let c = c1@c2@[(0, ASSIGNV (v, GT, v1, v2))] in
      (v, c)
    end
  | GE (e1, e2) -> begin
    let v = make_variable () in
    let (v1, c1) = translate_exp e1 in
    let (v2, c2) = translate_exp e2 in
    let c = c1@c2@[(0, ASSIGNV (v, GE, v1, v2))] in
      (v, c)
    end
  | EQ (e1, e2) -> begin
    let v = make_variable () in
    let (v1, c1) = translate_exp e1 in
    let (v2, c2) = translate_exp e2 in
    let c = c1@c2@[(0, ASSIGNV (v, EQ, v1, v2))] in
      (v, c)
    end
  | AND (e1, e2) -> begin
    let v = make_variable () in
    let (v1, c1) = translate_exp e1 in
    let (v2, c2) = translate_exp e2 in
    let c = c1@c2@[(0, ASSIGNV (v, AND, v1, v2))] in
      (v, c)
    end
  | OR (e1, e2) -> begin
    let v = make_variable () in
    let (v1, c1) = translate_exp e1 in
    let (v2, c2) = translate_exp e2 in
    let c = c1@c2@[(0, ASSIGNV (v, OR, v1, v2))] in
      (v, c)
    end
  end

let rec translate_stmt : S.stmt -> T.linstr list
= fun s -> begin match s with
  | ASSIGN (lv, e) -> begin match lv with
    | ID x -> begin
      let (v1, c1) = translate_exp e in
      let c = c1@[(0, COPY (x, v1))] in
        c
    end
    | ARR (x, ee) -> begin
      let (v1, c1) = translate_exp e in
      let (v2, c2) = translate_exp ee in
      let c = c1@c2@[(0, STORE ((x, v2), v1))] in
        c
    end
  end
  | IF (e, stmt1, stmt2) -> begin
    let (v1, c1) = translate_exp e in
    let ct = translate_stmt stmt1 in
    let cf = translate_stmt stmt2 in
    let lt = !label + 1 in
    let lf = !label + 2 in
    let lx = !label + 3 in
    let _ = (label := !label + 3) in
      c1@[(0, CJUMP (v1, lt))]@[(0, UJUMP lf)]@[(lt, SKIP)]@ct@[(0, UJUMP lx)]@[(lf, SKIP)]@cf@[(0, UJUMP lx)]@[(lx, SKIP)]
    end
  | WHILE (e, stmt) -> begin
    let (v1, c1) = translate_exp e in
    let cb = translate_stmt stmt in
    let le = !label + 1 in
    let lx = !label + 2 in
    let _ = (label := !label + 2) in
      [(le, SKIP)]@c1@[(0, CJUMPF (v1, lx))]@cb@[(0, UJUMP le)]@[(lx, SKIP)]
    end
  | DOWHILE (stmt, e) -> begin
    let (v1, c1) = translate_exp e in
    let cb = translate_stmt stmt in
    let le = !label + 1 in
    let lx = !label + 2 in
    let _ = (label := !label + 2) in
      cb@[(le, SKIP)]@c1@[(0, CJUMPF (v1, lx))]@cb@[(0, UJUMP le)]@[(lx, SKIP)]
    end
  | READ x -> [(0, READ x)]
  | PRINT e -> begin
    let (v1, c1) = translate_exp e in
    let c = c1@[(0, WRITE v1)] in
      c
    end
  | BLOCK b -> (translate_block b)
  end

and translate_decl : S.decl -> T.linstr list
= fun (typ, x) -> begin match typ with
  | TINT -> [(0, COPYC (x, 0))]
  | TARR n -> [(0, ALLOC (x, n))]
  end

and translate_decls : S.decls -> T.linstr list
= fun ds -> begin match ds with
  | [] -> []
  | hd::tl -> (translate_decl hd)@(translate_decls tl)
  end

and translate_stmts : S.stmts -> T.linstr list
= fun ss -> begin match ss with
  | [] -> []
  | hd::tl -> (translate_stmt hd)@(translate_stmts tl)
  end

and translate_block : S.block -> T.linstr list
= fun (decls, stmts) -> begin
  let decls_t = translate_decls decls in
  let stmts_t = translate_stmts stmts in
    decls_t@stmts_t 
  end

let translate : S.program -> T.program
= fun s -> (translate_block s)@[(0, HALT)]