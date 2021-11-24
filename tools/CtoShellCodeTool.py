import sys
import time
import pefile

class StringObj():
    def __init__(self,variable,data) -> None:
        self.variable = variable
        self.data = data
        self.index = None

## FILE IO
def read_dirty_file(path) -> list:
    with open(path,"r") as f:
            text = f.readlines()
    return text
    
def write_cleaned_file(path,text) -> int:
    with open(path,"w")as f:
        bytes_wrote = f.write(text)
    return bytes_wrote


### file string manipulation
def comment_includes(line) -> str:
    if "include" in line or "INCLUDELIB" in line:
        print("commented include")
        return ";" + line
    return line

def replace_short_jmps(line) -> str:
    if "SHORT" in line and "jmp" in line:
        print("replacing short jump")
        return line.replace("SHORT","")
    return line

def add_assume(line) -> str:
    if ".model	flat" in line:
        print("added assume")
        return  "	.model	flat\nassume fs:nothing\n"
    return line

def clean_line(line) -> str:
    line = add_assume(line)
    line = comment_includes(line)
    line = replace_short_jmps(line)
    return line

def get_string_variables(text) -> list:
    variables = []
    for x in text.split("\n"):
        if "$" in x and "DB" in x:
            variables.append(x.split("DB")[0].strip())
    return variables


def variable_string_code(variable,data_section,isLastLine) -> str:
    ## returns the variable code that needs to be pasted onto the text secton.
    data_section_lines = data_section.split(variable)
    start = data_section_lines[1]
    start_split = start.split("\n")
    end_line_index = 0
    for x in start_split:
        if "$" in x and "DB" in x:
            end_line_index = start_split.index(x)
            break
    end = start_split[end_line_index]
    if isLastLine:
        return "\n".join(start_split[0:end_line_index+1])
    return "\n".join(start_split[0:end_line_index])

def get_objects_and_data(full_text):
    full_text_lines = full_text.split("\n")
    ### split by data section we need.
    temp = full_text.split("_DATA	SEGMENT")[1]
    data_section = temp.split("_DATA	ENDS\n")[0]
    variables = get_string_variables(data_section)
    variables_and_code = []
    for counter,x in enumerate(variables):
        if counter == len(variables) - 1:
            variables_and_code.append(StringObj(x,variable_string_code(x,data_section,True)))
            continue
        variables_and_code.append(StringObj(x,variable_string_code(x,data_section,False)))

    # we need to get the line number of where we need to perform each swap
    # need to be second time we see the variable
    timesSeen = 0
    for var in variables_and_code:
        for count,line, in enumerate(full_text_lines):
            if var.variable in line:
                timesSeen +=1
            if timesSeen == 2:
                timesSeen = 0
                var.index = count
    return variables_and_code
                
def replace_strings(full_text,array_of_obj) -> str:
    full_text_lines = full_text.split("\n")
    for i,x in enumerate(array_of_obj):
        called = f"{x.variable[1:]}___"
        str_to_swap = f"call {called}\n{x.data}\n{called}:"
        lines_added = str_to_swap.count("\n")
        full_text_lines[x.index] = ";" + full_text_lines[x.index]
        full_text_lines.insert(x.index,str_to_swap)
        # increase location by one since we inserted to array
        for z in array_of_obj[i:]:
            z.index += 1
    return "\n".join(full_text_lines)
    
def x86_mode(path_to_asm,output):
    try:
        text = read_dirty_file(path_to_asm)
        fixed_text = "".join([clean_line(line) for line in text])
        array_of_strings_and_indexes = get_objects_and_data(fixed_text)
        inline_strings = replace_strings(fixed_text,array_of_strings_and_indexes)
        cleaned_file = "".join([clean_line(line) for line in inline_strings])
        wrote = write_cleaned_file(output,cleaned_file)
        print(f"Done! ")
    except Exception as e:
        print(f"Failed miserably -> {e}")


def x64_mode(path_to_asm,output):
    return None

def extract_mode(path_to_exe,output):
    pe = pefile.PE(path_to_exe)
    with open(output,"wb") as f:
        wrote = f.write(pe.sections[0].get_data())# zero is always text section pretty sure
    print(f"Wrote {wrote} bytes to -> {output}")


def usage(message):
    print(message)
    print("\tscript.py <mode> <file> <output>")
    print("\tmodes -> x64, x86, shellcode")
    quit()


if __name__ == "__main__":
    ### script.py <mode> <file> <output>
    if len(sys.argv) != 4:
        usage("Must be 3 args.")
    mode, in_, out_ = sys.argv[1], sys.argv[2], sys.argv[3]
    if mode == "x86":
        x86_mode(in_,out_)
    elif mode == "x64":
        quit()
    elif mode == "extract":
        extract_mode(in_,out_)
    else:
        usage("Mode must be x86,x64 or extract")

    
    