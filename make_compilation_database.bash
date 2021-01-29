#!/bin/bash
touch compilation_database/file_so_that_the_directory_is_not_empty
rm compilation_database/*;
for file in *.c; do
    clang -MJ "compilation_database/$(basename $file .c).o.json" -Wall -Werror -std=c99 -o "compilation_database/$(basename $file .c).o" -c "$(basename $file .c).c";
done
sed -e '1s/^/[/' -e '$s/,$/]/' compilation_database/*.o.json > compile_commands.json
