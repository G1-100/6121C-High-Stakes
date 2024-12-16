import re

def parse_api_definitions(lines):
    api_map = {}
    for line in lines:
        if line.startswith("api") or line.startswith("end"):
            continue
        cpp_function, command = line.split(";", 1)
        cpp_function = cpp_function.strip()
        command = command.strip().split()[0]  # Use only the base command
        api_map[command] = cpp_function
        print(f"Debug: Added API definition - Command: '{command}', Function: '{cpp_function}'")
    return api_map

def parse_routines(lines, api_map):
    routines = {}
    current_routine = None
    commands = []

    for line in lines:
        line = line.strip()
        if line.startswith("routine"):
            if current_routine:
                routines[current_routine] = commands
            current_routine = line.split()[1]
            commands = []
        elif line.startswith("end"):
            if current_routine:
                routines[current_routine] = commands
                current_routine = None
        elif line and not line.startswith("//"):
            commands.append(line)
    return routines

def translate_command(command, args, api_map):
    cmd_parts = command.split()
    base_cmd = cmd_parts[0]
    
    # Handle each command type specifically
    if base_cmd == "pose":
        x, y, theta = args
        return f"chassis.setPose({x}, {y}, {theta})"
    elif base_cmd == "Drive":
        if len(args) == 1:
            return f"set_drive({args[0]}, 3000, 0, 127)"
        elif len(args) == 2:
            return f"set_drive({args[0]}, {args[1]}, 0, 127)"
        elif len(args) == 3:
            return f"set_drive({args[0]}, {args[1]}, {args[2]}, 127)"
        else:
            return f"set_drive({', '.join(args)})"
    elif base_cmd == "Mogo":
        return "mogoClamp.toggle()"
    elif base_cmd == "TurnTo":
        x, y = args[0:2]
        ms = args[2] if len(args) > 2 else "3000"
        forward = args[3] if len(args) > 3 else "true"
        return f"chassis.turnToPoint({x}, {y}, {ms}, {{.forwards={forward}, .minSpeed=0, .maxSpeed=127}})"
    elif base_cmd == "LBExtend":
        return f"LBExtend({args[0]})"
    else:
        return f"// Unknown command: {command}"

def compile_routines_to_cpp(routines, api_map):
    cpp_code = []
    for routine_name, commands in routines.items():
        cpp_code.append(f"void {routine_name}() {{")
        for command in commands:
            parts = command.split()
            cmd_name = parts[0]
            cmd_args = parts[1:]
            translated = translate_command(cmd_name, cmd_args, api_map)
            cpp_code.append(f"    {translated};")
            # Add waitUntilDone for movement commands
            if cmd_name in ["Drive", "TurnTo"]:
                cpp_code.append("    chassis.waitUntilDone();")
        cpp_code.append("}")
        cpp_code.append("")
    return "\n".join(cpp_code)

def update_cpp_file(new_routines, api_map):
    # Read existing content
    try:
        with open("src/autons.cpp", "r") as file:
            lines = file.readlines()
    except FileNotFoundError:
        lines = []

    # Find and remove existing implementations while preserving includes and other content
    filtered_lines = []
    skip_mode = False
    for line in lines:
        if any(f"void {name}()" in line for name in new_routines.keys()):
            skip_mode = True
            continue
        if skip_mode and "}" in line:
            skip_mode = False
            continue
        if not skip_mode:
            filtered_lines.append(line)

    # Generate new implementations
    new_content = compile_routines_to_cpp(new_routines, api_map)
    
    # Ensure there are newlines between existing content and new content
    if filtered_lines and not filtered_lines[-1].strip() == "":
        filtered_lines.append("\n")
    if not new_content.startswith("\n"):
        filtered_lines.append("\n")
    
    # Write back to file
    with open("src/autons.cpp", "w") as impl_file:
        impl_file.writelines(filtered_lines)
        impl_file.write(new_content)

def main():
    with open("src/autons.dsl", "r") as file:
        syntax = file.read()

    lines = syntax.splitlines()
    api_start = lines.index("api")
    api_end = lines.index("end", api_start)

    api_lines = lines[api_start:api_end + 1]
    routine_lines = lines[api_end + 1:]

    api_map = parse_api_definitions(api_lines)
    print("Debug: Full API Map:", api_map)  # Added debug for full API map
    routines = parse_routines(routine_lines, api_map)
    cpp_code = compile_routines_to_cpp(routines, api_map)

    # Generate and write header file

    # Update implementation file
    cpp_code = update_cpp_file(routines, api_map)
    with open("src/autons.cpp", "w") as impl_file:
        impl_file.write(cpp_code)

if __name__ == "__main__":
    main()
