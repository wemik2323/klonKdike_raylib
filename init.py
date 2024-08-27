import subprocess
import os
import shutil
import re
import stat

def remove_readonly(func, path, _):
    os.chmod(path, stat.S_IWRITE)
    func(path)

def RunConan(build_type):
    subprocess.run((
        'conan', 'install', '.', 
        '--build', 'missing', 
        '--output-folder=./.conan', 
        '--settings=compiler.cppstd=20',
        f'--settings=build_type={build_type}'
    ))

def main():

    print();
    print("**********Установка имени проекта**********");
    workspace_name = input("workspace: ")
    project_name = input("project name: ")

    current_dir = os.getcwd()

    git_dir = os.path.join(current_dir, ".git")
    if os.path.isdir(git_dir):
        shutil.rmtree(git_dir, onerror=remove_readonly)
        print("Каталог .git удален.")
    else:
        print("Каталог .git не найден.")

    premake_file = os.path.join(current_dir, "premake5.lua")
    if os.path.isfile(premake_file):
        with open(premake_file, 'r', encoding='utf-8') as file:
            content = file.read()

        content = re.sub(r'workspace\s*".*"', f'workspace "{workspace_name}"', content)
        content = re.sub(r'project\s*".*"', f'project "{project_name}"', content)

        with open(premake_file, 'w', encoding='utf-8') as file:
            file.write(content)

        print("Файл premake5.lua обновлен.")
    else:
        print("Файл premake5.lua не найден.")

if __name__ == "__main__":
    RunConan("Debug")
    RunConan("Release")
    main()



