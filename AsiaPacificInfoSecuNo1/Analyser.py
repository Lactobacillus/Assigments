# Analyser.py
# Taint Analysis
# Asia Pacific Information Security Number One
# 2016.11.27

# Code was written for Python3
# libraries
# 1. plyj
# 	in Linux : sudo pip install plyj
# 	in Windows : pip install plyj

try:

	import sys
	import codecs
	import inspect
	import plyj as PLYJ
	import plyj.parser as plyj

except:

	print('Library Error : plyj')
	sys.exit(0)

# initialize variable for analysis
# type set for Set Based Solution
type_set = dict()
# temp variable for counting instance
count = [0]
# case list for constraints
case = [None, list(), list(), list(), list(), list()]
	# case 1 : qx < qy
	# case 2 : qx < qy tri qz
	# case 3 : qx tri qy < qz
	# case 4 : qx < q^i tri qthis
	# case 5 : q^i tri qret < q x
# parameter list of each method
params = dict()
# list of safe or tainted method
safe_list = list()
tainted_list = list()
# list of method and variable pair
pre_safe = list()
pre_tainted = list()
# error list for candidates
error = list()
# list for tracing error
trace = list()

def getSafeList():

	# open Safe List
	try:

		with codecs.open('safe list.txt', 'r', encoding = 'UTF-8') as f:

			safe = f.readlines()


		for s in safe:

			safe_list.append(s.replace('\n', '').replace('\r', ''))

	except:

		print('Can not open safe list file')
		sys.exit(0)

def getTaintList():

	# open Tainted List
	try:

		with codecs.open('tainted list.txt', 'r', encoding = 'UTF-8') as f:

			tainted = f.readlines()

		for t in tainted:

			tainted_list.append(t.replace('\n', '').replace('\r', ''))

	except:

		print('Can not open tainted list file')
		sys.exit(0)

def openSourceCode(fileName):

	# open Source Code
	try:

		with codecs.open(fileName, 'r', encoding = 'UTF-8') as f:

			code = f.read()

		return code

	except:

		print('Can not open source file')
		sys.exit(0)

def paddingSet():

	for [x, y] in case[1]:

		if x not in type_set:

			type_set[x] = {'T', 'P', 'S'}

		if y not in type_set:

			type_set[y] = {'T', 'P', 'S'}

	for [x, y, z] in case[2]:

		if x not in type_set:

			type_set[x] = {'T', 'P', 'S'}

		if y not in type_set:

			type_set[y] = {'T', 'P', 'S'}

		if z not in type_set:

			type_set[z] = {'T', 'P', 'S'}

	for [x, y, z] in case[3]:

		if x not in type_set:

			type_set[x] = {'T', 'P', 'S'}

		if y not in type_set:

			type_set[y] = {'T', 'P', 'S'}

		if z not in type_set:

			type_set[z] = {'T', 'P', 'S'}

	for [x, i, this] in case[4]:

		if x not in type_set:

			type_set[x] = {'T', 'P', 'S'}

		if this not in type_set:

			type_set[this] = {'T', 'P', 'S'}

	for [i, ret, x] in case[5]:

		if ret not in type_set:

			type_set[ret] = {'T', 'P', 'S'}

		if x not in type_set:

			type_set[x] = {'T', 'P', 'S'}

def getPrameterNames(tree):

	if isinstance(tree, str):
		# pass useless node
		pass

	elif len(tree._fields) > 0:
		# check child node is exists
		for field in tree._fields:

			attrs = getattr(tree, field)

			if isinstance(attrs, list):

				for child in getattr(tree, field):

					if isinstance(child, PLYJ.model.MethodDeclaration):
						# get parameters of method
						params[child.name] = list()

						for parameter in child.parameters:

							params[child.name].append(parameter.variable.name)

					else:

						getPrameterNames(child)

	else:

		pass

def analyser(tree, class_name = None, method_name = None, var_type = None):

	if isinstance(tree, str):
		# pass useless node
		pass

	elif len(tree._fields) > 0:
		# check child node is exists
		for field in tree._fields:

			attrs = getattr(tree, field)

			if isinstance(attrs, list):

				for child in getattr(tree, field):

					if isinstance(child, PLYJ.model.MethodInvocation):
						# make constraint
						m = child.name

						try:

							y = child.target.value

							try:

								z = child.arguments[0].value

								if m in tainted_list:

									type_set[method_name + '_' + z] = {'T'}
									pre_tainted.append([method_name + '_' + z, m])

								elif m in safe_list:

									type_set[method_name + '_' + z] = {'S'}
									pre_safe.append([method_name + '_' + z, m])

								case[4].append([method_name + '_' + y, count[0], 'this_' + m])
								case[4].append([method_name + '_' + z, count[0], m + '_' + params[m][0]])
								case[5].append([count[0], 'this_' + m, method_name + '_' + y])
								count[0] = count[0] + 1

							except:

								case[4].append([method_name + '_' + y, count[0], 'this_' + m])
								count[0] = count[0] + 1

						except:

							try:

								z = child.arguments[0].value

								if m in tainted_list:

									type_set[method_name + '_' + z] = {'T'}
									pre_tainted.append([method_name + '_' + z, m])

								elif m in safe_list:

									type_set[method_name + '_' + z] = {'S'}
									pre_safe.append([method_name + '_' + z, m])

								case[4].append(method_name + '_' + z, count[0], m + '_' + params[m][0])

							except:

								pass

					elif isinstance(child, PLYJ.model.FormalParameter):
						# set base : parameter
						if method_name in tainted_list:

							type_set[method_name + '_' + child.variable.name] = {'T'}

						elif method_name in safe_list:

							type_set[method_name + '_' + child.variable.name] = {'S'}

						else:

							type_set[method_name + '_' + child.variable.name] = {'T', 'P', 'S'}

					elif isinstance(child, PLYJ.model.MethodDeclaration):

						if child.name in tainted_list:

							type_set['this_' + child.name] = {'T'}

						elif child.name in safe_list:

							type_set['this_' + child.name] = {'S'}

						else:

							type_set['this_' + child.name] = {'T', 'P', 'S'}

						analyser(child, class_name, child.name, var_type)

					elif isinstance(child, PLYJ.model.Return):

						if method_name in tainted_list:
							type_set['return_' + method_name] = {'T'}

						elif method_name in safe_list:
							type_set['return_' + method_name] = {'S'}

						else:
							type_set['return_' + method_name] = {'T', 'P', 'S'}

						if isinstance(child.result,PLYJ.model.Name):

						 	case[1].append([method_name + '_' + child.result.value, 'return_' + method_name])

						elif isinstance(child.result,PLYJ.model.FieldAccess):

							if child.result.target == 'this':

								case[3].append(['this_' + method_name, child.result.name, 'return_' + method_name])

							else:

								case[3].append([child.result.target, child.result.name, 'return_' + method_name])

						elif isinstance(child.result,PLYJ.model.MethodInvocation):

							m = child.result.name

							if m in tainted_list:

								type_set['return_' + method_name] = {'T'}
								pre_tainted.append(['return_' + method_name, m])

							elif m in safe_list:

								type_set['return_' + method_name] = {'S'}
								pre_safe.append(['return_' + method_name, m])

							try:

								y = child.result.target.value

								try:

									z = child.result.arguments[0].value

									if m in tainted_list:

										type_set[method_name + '_' + z] = {'T'}
										pre_tainted.append([method_name + '_' + z, m])

									elif m in safe_list:

										type_set[method_name + '_' + z] = {'S'}
										pre_safe.append([method_name + '_' + z, m])

									case[4].append([method_name + '_' + y, count[0], 'this_' + m])
									case[4].append(method_name + '_' + z, count[0], m + '_' + params[m][0])
									case[5].append([count[0], 'this_' + m, method_name + '_' + y])
									count[0] = count[0] + 1

								except:

									case[4].append([method_name + '_' + y, count[0], 'this_' + m])
									case[5].append([count[0], 'return_' + m, 'return_' + method_name])
									count[0] = count[0] + 1

							except:

								try:

									z = child.result.arguments[0].value

									if m in tainted_list:

										type_set[method_name + '_' + z] = {'T'}
										pre_tainted.append([method_name + '_' + z, m])

									elif m in safe_list:

										type_set[method_name + '_' + z] = {'S'}
										pre_safe.append([method_name + '_' + z, m])

									case[4].append(method_name + '_' + z, count[0], m + '_' + params[m][0])
									case[5].append([count[0], 'return_' + m,'return_' + method_name])
									count[0] = count[0] + 1

								except:

									case[5].append([count[0], 'return_' + m, 'return_' + method_name])
									count[0] = count[0] + 1

					elif isinstance(child, PLYJ.model.ClassDeclaration):

						analyser(child, child.name, method_name, var_type)

					elif isinstance(child, PLYJ.model.FieldDeclaration) and \
						 not isinstance(child, PLYJ.model.VariableDeclaration):

						analyser(child, class_name, method_name, 'field')

					elif isinstance(child, PLYJ.model.VariableDeclarator):

						if method_name != None:
							# set base : variable
							var_type = 'variable'
							if method_name in tainted_list:

								type_set[method_name + '_' + child.variable.name] = {'T'}

							elif method_name in safe_list:

								type_set[method_name + '_' + child.variable.name] = {'S'}

							else:

								type_set[method_name + '_' + child.variable.name] = {'T', 'S', 'P'}

						else :
							# set base : field
							type_set[child.variable.name] = {'T', 'P'}

						try:

							if isinstance(child.initializer,PLYJ.model.Name):

								if child.initializer.value.find('.') == -1:
									# make Constraint
									# x = y, x = y.f
									x = child.variable.name
									y = child.initializer.value
									# child.initializer.value = y, child.variable.name = x
									case[1].append([method_name + '_' + y, method_name + '_' + x])

								else:

									x = child.variable.name
									y = child.initializer.value.split('.')[0]
									f = child.initializer.value.split('.')[1]
									# child.initializer.value = y.f
									case[3].append([method_name + '_' + y, y + '_' + f, method_name + '_' + x])

						except:

							pass

						try:

							if isinstance(child.initializer,PLYJ.model.FieldAccess):
								# make Constraint
								x = child.variable.name
								y = child.initializer.target
								f = child.initializer.value

								case[3].append([method_name + '_' + y, y + '_' + f, method_name + '_' + x])

						except:

							pass

						#try:
						#
						#	if isinstance(child.initializer,PLYJ.model.InstanceCreation):
						#		# make Constraint
						#		# x = new q C
						#		x = child.variable.name
						#		C = child.initializer.name.value # ?? q를 어떻게구하지
						#
						#except:
						#
						#	pass

						try:

							if isinstance(child.initializer,PLYJ.model.MethodInvocation): # x=y.m^i(z)
								# make Constraint
								x = child.variable.name
								m = child.initializer.name

								if m in tainted_list:

									type_set[method_name + '_' + child.variable.name] = {'T'}
									pre_tainted.append([method_name + '_' + child.variable.name, m])

								elif m in safe_list:

									type_set[method_name + '_' + child.variable.name] = {'S'}
									pre_safe.append([method_name + '_' + child.variable.name, m])

								try:

									y = child.initializer.target.value

									try:

										z = child.initializer.arguments[0].value

										if m in tainted_list:

											type_set[method_name + '_' + z] = {'T'}
											pre_tainted.append([method_name + '_' + z, m])

										elif m in safe_list:

											type_set[method_name + '_' + z] = {'S'}
											pre_safe.append([method_name + '_' + z, m])

										case[4].append([method_name + '_' + y, count[0], 'this_' + m])
										case[4].append(method_name + '_' + z, count[0] , m + '_' + params[m][0])
										case[5].append([count[0], 'return_' + m, method_name + '_' + x])
										count[0] = count[0] + 1

									except:

										case[4].append([method_name + '_' + y, count[0], 'this_' + m])
										case[5].append([count[0], 'return_' + m, method_name + '_' + x])
										count[0] = count[0] + 1

								except:

									try:

										z = child.initializer.arguments[0].value

										if m in tainted_list:

											type_set[method_name + '_' + z] = {'T'}
											pre_tainted.append([method_name + '_' + z, m])

										elif m in safe_list:

											type_set[method_name + '_' + z] = {'S'}
											pre_safe.append([method_name + '_' + z, m])

										case[4].append(method_name + '_' + z, count[0], m + '_' + params[m][0])
										case[5].append([count[0], 'return_' + m, method_name + '_' + x])
										count[0] = count[0] + 1

									except:

										case[5].append([count[0], 'return_' + m, method_name + '_' + x])
										count[0] = count[0] + 1

						except:

							pass

					elif isinstance(child,PLYJ.model.Assignment):
						# make Constraint
						if isinstance(child.lhs,PLYJ.model.FieldAccess):
							# y.f = x
							x = child.rhs.value
							y = child.lhs.target
							f = child.lhs.name

							if y == 'this':

								case[2].append([method_name + '_' + x, 'this_' + method_name, f])

							else:

								case[2].append([method_name + '_' + x, method_name + '_' + y, y + '_' + f])

						elif isinstance(child.lhs,PLYJ.model.Name):

							if isinstance(child.rhs,PLYJ.model.Name):
								# x = y, x = y.f
								x = child.lhs.value

								if child.rhs.value.find('.') == -1:

									y = child.rhs.value

									case[1].append([method_name + '_' + y, method_name + '_' + x])

								else:

									y = child.rhs.value.split('.')[0]
									f = child.rhs.value.split('.')[1]

									case[3].append([method_name + '_' + y, y + '_' + f, method_name + '_' + x])

						#elif isinstance(child.rhs,PLYJ.model.InstanceCreation):
						#	# x = new q C
						#	x = child.lhs.value
						#	C = child.rhs.name.value

						elif isinstance(child.rhs,PLYJ.model.MethodInvocation):
							# x = y.m^i(z)
							x = child.lhs.value
							m = child.rhs.name

							if m in tainted_list:

								type_set[method_name + '_' + child.variable.name] = {'T'}
								pre_tainted.append([method_name + '_' + child.variable.name, m])

							elif m in safe_list:

								type_set[method_name + '_' + child.variable.name] = {'S'}
								pre_safe.append([method_name + '_' + child.variable.name, m])

							try:

								y = child.rhs.target.value

								try:

									z = child.rhs.arguments[0].value

									if m in tainted_list:

										type_set[method_name + '_' + z] = {'T'}
										pre_tainted.append([method_name + '_' + z, m])

									elif m in safe_list:

										type_set[method_name + '_' + z] = {'S'}
										pre_safe.append([method_name + '_' + z, m])

									case[4].append([method_name + '_' + y, count[0], 'this_' + m])
									case[4].append(method_name + '_' + z, count[0], m + '_' + params[m][0])
									case[5].append([count[0], 'return_' + m, method_name + '_' + x])
									count[0] = count[0] + 1

								except:

									case[4].append([method_name + '_' + y, count[0], 'this_' + m])
									case[5].append([count[0], 'return_' + m, method_name + '_' + x])
									count[0] = count[0] + 1

							except:

								try:

									z = child.rhs.arguments[0].value

									if m in tainted_list:

										type_set[method_name + '_' + z] = {'T'}
										pre_tainted.append([method_name + '_' + z, m])

									elif m in safe_list:

										type_set[method_name + '_' + z] = {'S'}
										pre_safe.append([method_name + '_' + z, m])

									case[4].append(method_name + '_' + z, count[0], m + '_' + params[m][0])
									case[5].append([count[0], 'return_' + m, method_name + '_' + x])
									count[0] = count[0] + 1

								except:

									case[5].append([count[0], 'return_' + m, method_name + '_' + x])
									count[0] = count[0] + 1

					else:

						analyser(child, class_name, method_name, var_type)

	else:

		pass

def _remove(t_set, var_name, * elements):
	# internal function of _solve
	# remove some elements e from type_set[x] if exists
	changed = False

	for element in elements:

		if element in t_set[var_name]:

			t_set[var_name].remove(element)
			changed |= True

	return changed

def _maxT(t_set, var_name):
	# internal function of _solve
	# return largest type of variable (t_set[var_name])
	# if x is empty, it means type error
	if 'T' in t_set[var_name]:

		return 'T'

	elif 'P' in t_set[var_name]:

		return 'P'

	elif 'S' in t_set[var_name]:

		return 'S'

	else:
		# type error
		return None

def _minT(t_set, var_name):
	# internal function of _solve
	# return smallest type of variable (t_set[var_name])
	# if x is empty, it means type error
	if 'S' in t_set[var_name]:

		return 'S'

	elif 'P' in t_set[var_name]:

		return 'P'

	elif 'T' in t_set[var_name]:

		return 'T'

	else:
		# type error
		return None

def _solve(type_set, case, error, trace):
	# internal function of solver
	solver_changed = True

	while solver_changed:

		solver_changed = False

		for [x, y] in case[1]:

			if _maxT(type_set, y) == 'S':

				is_removed = _remove(type_set, x, 'P', 'T')
				solver_changed |= is_removed

				if is_removed:

					trace.append([x, y])

			elif _maxT(type_set, y) == 'P':

				is_removed = _remove(type_set, x, 'T')
				solver_changed |= is_removed

				if is_removed:

					trace.append([x, y])

			if _minT(type_set, x) == 'P':

				is_removed = _remove(type_set, y, 'S')
				solver_changed |= is_removed

				if is_removed:

					trace.append([y, x])

			elif _minT(type_set, x) == 'T':

				is_removed = _remove(type_set, y, 'S', 'P')
				solver_changed |= is_removed

				if is_removed:

					trace.append([y, x])

			if _maxT(type_set, x) == None or _maxT(type_set, y) == None:

				if len(error) == 0:

					error.extend([x, y])

		for [x, y, z] in (case[3] + case[5]):

			if _maxT(type_set, z) == 'S' or _maxT(type_set, z) == 'P':

				is_removed = _remove(type_set, y, 'T')
				solver_changed |= is_removed

				if is_removed:

					trace.append([y, z])

			if _minT(type_set, y) == 'T':

				is_removed = _is_removedmove(type_set,z,'S','P')
				solver_changed |= is_removed

				if is_removed:

					trace.append([z, y])

			if _maxT(type_set, y) == None or _maxT(type_set, z) == None:

				if len(error) == 0:

					error.extend([x,z])

		for [x, y, z] in (case[2] + case[4]):

			if _maxT(type_set, z) == 'S':

				is_removed = _remove(type_set, x, 'T', 'P')
				solver_changed |= is_removed

				if is_removed:

					trace.append([x, z])

			if _minT(type_set, x) == 'T' or _minT(type_set, x) == 'P':

				is_removed = _remove(type_set, z, 'S')
				solver_changed |= is_removed

				if is_removed:

					trace.append([z, x])

			if _maxT(type_set, x) == None or _maxT(type_set, z) == None:

				if len(error) == 0:

					error.extend([x, z])

def solver(t_set, case, error, trace):
	# main solver
	changed = True

	while changed:

		_solve(t_set, case, error, trace)
		changed = False

		for c in case[2]:

			if t_set[c[2]] == {'P'} and [c[0], c[1]] not in case[1]:

				case[1].append([c[0], c[1]])
				changed |= True

		for c in case[3]:

			if t_set[c[1]] == {'P'} and [c[0], c[2]] not in case[1]:

				case[1].append([c[0], c[2]])
				changed |= True

		for c in case[1]:

			for z in case[1]:

				if c[1] == z[0] and [c[0], z[1]] not in case[1]:

					case[1].append([c[0], z[1]])
					changed |= True

			for w in case[1]:

				if c[0] == w[1] and [w[0], c[1]] not in case[1]:

					case[1].append([w[0], c[1]])
					changed |= True

			for w in case[4]:

				for z in case[5]:

					if w[2] == c[0] and z[1] == c[1] and w[1] == z[0] and [w[0], z[2]] not in case[1]:

						case[1].append([w[0], z[2]])
						changed |= True

def _findIdx(pre_type, var_name):
	# internal function of traceError
	idx = -1

	for [x, y] in pre_type:

		idx += 1

		if x == var_name:

			return idx

	return idx

def _Ltrace(trace, error, p_tainted):
	# internal function of traceError
	if _findIdx(p_tainted, error[0]) >= 0:
		
		return ([p_tainted[_findIdx(p_tainted, error[0])][1]] + error, True)
	
	for t in trace:
		
		if t[0] == error[0] and t[1] not in error:
			
			(error2, result2) = _Ltrace(trace, [t[1]] + error, p_tainted)
			
			if result2 :
				
				return (error2, True)
	
	return (error, False)

def _Rtrace(trace, error, p_safe):
	# internal function of traceError
	if _findIdx(p_safe, error[-1]) >= 0:
		
		return (error + [p_safe[_findIdx(p_safe, error[-1])][1]], True)
	
	for t in trace:
	
		if t[1] == error[-1] and t[0] not in error:
	
			(error2, result2) = _Rtrace(trace, error + [t[0]], p_safe)
	
			if result2:
	
				return (error2, True)
	
	return (error, False)

def traceError(trace, error, p_tainted, p_safe):

	(error, result) = _Ltrace(trace, error, p_tainted)
	(error, result) = _Rtrace(trace, error, p_safe)
	
	return error

def showSet(t_set):

	for key, value in t_set.items():

		print(key, value)

	for c in case:

		print(c)

def showResult():

	print('\n##### Result of Analysis #####')

	if len(error) == 0:

		print('Congratuation : no error in your source code')

	else:

		for e in error:

			print('Error in : ' + e)

		print('')
		answer = traceError(trace, error, pre_tainted, pre_safe)

		for idx in range(0, len(answer)):

			if idx != len(answer) - 1:

				print(answer[idx], end = ' => ')

			else:

				print(answer[idx])

def main():

	# load parsing library
	parser = plyj.Parser()

	# load safe, tainted method list
	getSafeList()
	getTaintList()

	# load input source code
	input_code = openSourceCode(sys.argv[1])

	# make parse tree from source code
	parse_tree = parser.parse_string(input_code)

	try:

		# taint analysis of source code
		getPrameterNames(parse_tree)
		analyser(parse_tree)
		paddingSet()
		solver(type_set, case, error, trace)

		# result of analysis
		showResult()

	except:

		print('Parse Error : check syntax error in source code')
		sys.exit(0)

if __name__ == '__main__':

	if len(sys.argv) != 2:

		print('Usage : python Analyser.py <filename>')
		sys.exit(0)

	else:
		# call main function
		main()
