def convert_to_c_str(file, var):
    with open(file, 'r') as f:
        content = f.read()

        c_str_content = content.replace('"', '\\"').replace('\n', '\\n"\n"')
        with open(f"{var}.h", 'w') as output_file:
            output_file.write(f'const char {var}[] PROGMEM = R"rawliteral(\n"{c_str_content}\n)rawliteral";\n');

convert_to_c_str('index.html', 'index_html')
