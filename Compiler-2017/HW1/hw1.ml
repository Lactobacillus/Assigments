(*
* Homwork #1
* Compiler 2017
* Author : Cho Youngwoo
* 2017.03.31
*)

open Regex

exception Not_implemented
exception Nice_Nice_Good_Good

(* get state from (Nfa.state * Nfa.t) *)
let get_s : Nfa.state * Nfa.t -> Nfa.state
= fun (s, t) -> s

(* get t from (Nfa.state * Nfa.t) *)
let get_t : Nfa.state * Nfa.t -> Nfa.t
= fun (s, t) -> t

(* merge nfa1 and nfa2 to nfa without initial state, final state *)
let merge_nfa : Nfa.t -> Nfa.t -> Nfa.t -> Nfa.t
= fun nfa nfa1 nfa2 ->
  let copy_states = Nfa.add_states nfa (BatSet.union (Nfa.get_states nfa1) (Nfa.get_states nfa2)) in
  let edge_nfa1 = Nfa.get_edges nfa1 in
  let edge_nfa2 = Nfa.get_edges nfa2 in
  let con_edge = (fun (e1, ee1) (e2, ee2) -> e1@e2) edge_nfa1 edge_nfa2 in
  let con_eps_edge = (fun (e1, ee1) (e2, ee2) -> ee1@ee2) edge_nfa1 edge_nfa2 in
  let copy_edges = Nfa.add_edges copy_states (con_edge, con_eps_edge) in
    copy_edges

(* internal function of regex2nfa using recursion *)
let rec make_nfa : Regex.t -> Nfa.t
= fun regex -> match regex with
  | Empty ->
    let temp_nfa = Nfa.create_new_nfa () in
    (* let source = Nfa.get_initial_state temp_nfa in *)
    let sink = Nfa.create_state temp_nfa in
    let final_nfa = Nfa.add_final_state (get_t sink) (get_s sink) in
      final_nfa
  | Epsilon ->
    let temp_nfa = Nfa.create_new_nfa () in
    let source = Nfa.get_initial_state temp_nfa in
    let sink = Nfa.create_state temp_nfa in
    let connect = Nfa.add_epsilon_edge (get_t sink) (source, (get_s sink)) in
    let final_nfa = Nfa.add_final_state connect (get_s sink) in
      final_nfa
  | Alpha a ->
    let temp_nfa = Nfa.create_new_nfa () in
    let source = Nfa.get_initial_state temp_nfa in
    let sink = Nfa.create_state temp_nfa in
    let connect = Nfa.add_edge (get_t sink) (source, a, (get_s sink)) in
    let final_nfa = Nfa.add_final_state connect (get_s sink) in
      final_nfa
  | OR (t1, t2) ->
    let temp_nfa = Nfa.create_new_nfa () in
    let source = Nfa.get_initial_state temp_nfa in
    let sink = Nfa.create_state temp_nfa in
    let t1_nfa = make_nfa t1 in
    let t2_nfa = make_nfa t2 in
    let copy_nfa = merge_nfa (get_t sink) t1_nfa t2_nfa in
    let con_source_t1 = Nfa.add_epsilon_edge copy_nfa (source, (Nfa.get_initial_state t1_nfa)) in
    let con_source_t2 = Nfa.add_epsilon_edge con_source_t1 (source, (Nfa.get_initial_state t2_nfa)) in
    let con_t1_sink = Nfa.add_epsilon_edge con_source_t2 ((BatSet.choose (Nfa.get_final_states t1_nfa)), (get_s sink)) in
    let con_t2_sink = Nfa.add_epsilon_edge con_t1_sink ((BatSet.choose (Nfa.get_final_states t2_nfa)), (get_s sink)) in
    let final_nfa = Nfa.add_final_state con_t2_sink (get_s sink) in
      final_nfa
  | CONCAT (t1, t2) ->
    let temp_nfa = Nfa.create_new_nfa () in
    let source = Nfa.get_initial_state temp_nfa in
    let sink = Nfa.create_state temp_nfa in
    let t1_nfa = make_nfa t1 in
    let t2_nfa = make_nfa t2 in
    let copy_nfa = merge_nfa (get_t sink) t1_nfa t2_nfa in
    let con_source_t1 = Nfa.add_epsilon_edge copy_nfa (source, (Nfa.get_initial_state t1_nfa)) in
    let con_t1_t2 = Nfa.add_epsilon_edge con_source_t1 ((BatSet.choose (Nfa.get_final_states t1_nfa)), (Nfa.get_initial_state t2_nfa)) in
    let con_t2_sink = Nfa.add_epsilon_edge con_t1_t2 ((BatSet.choose (Nfa.get_final_states t2_nfa)), (get_s sink)) in
    let final_nfa = Nfa.add_final_state con_t2_sink (get_s sink) in
      final_nfa
  | STAR t ->
    let temp_nfa = Nfa.create_new_nfa () in
    let source = Nfa.get_initial_state temp_nfa in
    let sink = Nfa.create_state temp_nfa in
    let small_nfa = make_nfa t in
    let backward = Nfa.add_epsilon_edge small_nfa ((BatSet.choose (Nfa.get_final_states small_nfa)), (Nfa.get_initial_state small_nfa)) in
    let cp_states = Nfa.add_states (get_t sink) (Nfa.get_states backward) in
    let cp_edges = Nfa.add_edges cp_states (((fun (e1, ee1) -> e1) (Nfa.get_edges backward)), ((fun (e1, ee1) -> ee1) (Nfa.get_edges backward))) in 
    let con_source_t = Nfa.add_epsilon_edge cp_edges (source, (Nfa.get_initial_state backward)) in
    let con_t_sink = Nfa.add_epsilon_edge con_source_t ((BatSet.choose (Nfa.get_final_states backward)), (get_s sink)) in
    let jump = Nfa.add_epsilon_edge con_t_sink (source, (get_s sink)) in
    let final_nfa = Nfa.add_final_state jump (get_s sink) in
      final_nfa

(* make nfa using make_nfa *)
let regex2nfa : Regex.t -> Nfa.t 
= fun regex ->
  let nfa = make_nfa regex in
    nfa

(* internal function of closure using recursion *)
let rec _closure : Nfa.t -> Nfa.state list -> Regex.alphabet -> Nfa.states
= fun nfa states a -> match states with
  | [] -> BatSet.empty
  | hd::tl -> BatSet.union (Nfa.get_next_state nfa hd a) (_closure nfa tl a)

(* find closure of states respect to alphabet using _closure *)
let closure : Nfa.t -> Nfa.states -> Regex.alphabet -> Nfa.states
= fun nfa states a ->
  let result = _closure nfa (BatSet.to_list states) a in
    result

(* internal function of eps_closure using recursion*)
let rec _eps_closure : Nfa.t -> Nfa.state list -> Nfa.states
= fun nfa states -> match states with
  | [] -> BatSet.empty
  | hd::tl -> BatSet.add hd (BatSet.union (Nfa.get_next_state_epsilon nfa hd) (_eps_closure nfa tl))

(* find epsilon-closure of states using _eps_closure *)
let rec eps_closure : Nfa.t -> Nfa.states -> Nfa.states
= fun nfa states ->
  let temp = _eps_closure nfa (BatSet.to_list states) in
  let result = (fun n s t -> match BatSet.equal s t with
    | true -> t
    | false -> eps_closure n (BatSet.union s t)
  ) nfa states temp in
    result

(* add final state to dfa *)
let add_final_state_dfa : Nfa.t -> Dfa.t -> Dfa.state -> Dfa.t
= fun nfa dfa state -> match BatSet.is_empty (BatSet.intersect (Nfa.get_final_states nfa) state) with
  | true -> dfa
  | false -> Dfa.add_final_state dfa state

(* internal function of nfa2dfa using recursion *)
let rec make_dfa : Nfa.t -> Dfa.t -> Dfa.states -> Dfa.states -> Dfa.t
= fun nfa dfa ss ws -> match BatSet.is_empty ws with
  | true -> dfa
  | false ->
    let state, new_ws = BatSet.pop ws in
    let for_a, a_ss, a_ws = (fun n d s t_ss ->
      let temp = eps_closure n (closure n s A) in
      let new_state_set = BatSet.add temp t_ss in
      let state_dfa = Dfa.add_state d temp in
      let edge_dfa = Dfa.add_edge state_dfa (s, A, temp) in
      let final_state_dfa = add_final_state_dfa nfa edge_dfa temp in
      let candidate = (match BatSet.equal t_ss new_state_set with
        | true -> BatSet.empty
        | false -> BatSet.singleton temp
      ) in
        (final_state_dfa, new_state_set, candidate)
    ) nfa dfa state ss in
    let for_b, b_ss, b_ws = (fun n d s t_ss ->
      let temp = eps_closure n (closure n s B) in
      let new_state_set = BatSet.add temp t_ss in
      let state_dfa = Dfa.add_state d temp in
      let edge_dfa = Dfa.add_edge state_dfa (s, B, temp) in
      let final_state_dfa = add_final_state_dfa nfa edge_dfa temp in
      let candidate = (match BatSet.equal t_ss new_state_set with
        | true -> BatSet.empty
        | false -> BatSet.singleton temp
      ) in
        (final_state_dfa, new_state_set, candidate)
    ) nfa for_a state a_ss in
    let final_ws = BatSet.union b_ws (BatSet.union a_ws new_ws) in
    let final_dfa = make_dfa nfa for_b b_ss final_ws in
      final_dfa

(* make dfa using make_dfa *)
let nfa2dfa : Nfa.t -> Dfa.t
= fun nfa ->
  let initial_state = eps_closure nfa (BatSet.singleton (Nfa.get_initial_state nfa)) in
  let initial_dfa = Dfa.create_new_dfa initial_state in
  let state_set = BatSet.singleton initial_state in
  let working_set = BatSet.singleton initial_state in
  let temp_dfa = make_dfa nfa initial_dfa state_set working_set in
  let dfa = add_final_state_dfa nfa temp_dfa initial_state in
    dfa

(* make regex to dfa using regex2nfa and nfa2dfa *)
let regex2dfa : Regex.t -> Dfa.t
= fun regex ->
  let nfa = regex2nfa regex in
  let dfa = nfa2dfa nfa in
  (*let _ = Nfa.print nfa in*)
  (*let _ = Dfa.print dfa in*)
    dfa

(* internal function of run_dfa using recursion *)
let rec transition : Dfa.t -> Dfa.state -> alphabet list -> Dfa.state
= fun dfa state str -> match str with
  | [] -> state
  | hd::tl -> transition dfa (Dfa.get_next_state dfa state hd) tl

(* run dfa with string using transition *)
let run_dfa : Dfa.t -> alphabet list -> bool
= fun dfa str ->
  let state = Dfa.get_initial_state dfa in
  let after_transition = transition dfa state str in
  let accept = Dfa.is_final_state dfa after_transition in
    accept