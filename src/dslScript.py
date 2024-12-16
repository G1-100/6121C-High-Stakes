import re

def compile_dsl_to_cpp(dsl_code):
    cpp_code = []
    lines = dsl_code.splitlines()
    function_name = lines[0].strip()#
    cpp_code.append(f"void {function_name}() {{")

    for line in lines[1:]:
        line = line.strip()
        if line.startswith("//") or not line:
            continue  # Skip comments and empty lines

        # Match specific commands
        if line.startswith("pose"):
            args = line.split()[1:]
            cpp_code.append(f"    chassis.setPose({', '.join(args)});")
        elif line.startswith("Ladybrown"):
            args = line.split()[1:]
            cpp_code.append(f"    LBExtend({', '.join(args)});")
        elif line.startswith("Mogo"):
            cpp_code.append(f"    mogoClamp.toggle();")
        elif line.startswith("Drive"):
            args = line.split()[1:]
            cpp_code.append(f"    set_drive({', '.join(args)});")
        elif line.startswith("TurnTo"):
            args = line.split()[1:]
            cpp_code.append(
                f"    chassis.turnToPoint({args[0]}, {args[1]}, {args[2]}, "
                f"{{{'true' if args[3] == 'true' else 'false'}, {args[4]}, {args[5]}}});"
            )
        else:
            raise SyntaxError(f"Unknown command: {line}")

    cpp_code.append("}")
    return "\n".join(cpp_code)


# Example Usage
dsl_script = """\
ringRushAuton
pose 30 60 90
Ladybrown 1
Mogo
Drive 12
Ladybrown 2
Drive -6 1500 20
TurnTo 12 24 3000 false 80 90
Ladybrown 0
"""

compiled_code = compile_dsl_to_cpp(dsl_script)
print(compiled_code)
