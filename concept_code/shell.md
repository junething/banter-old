# comparision of some semi-advanced common shell tasks in different langagues
### bash
12 LOC
8 SLOC
```bash
for file in * do
	case "${filename##*.}" in
		txt)
			# stuff
		;;
		md)
			# stuff
		;;
		png | jpg)
			# stuff
		;;
	esac
```
### fish
9 LOC
8 SLOC
```bash
for file in *
	switch (string split -m1 -r '.' "$f")[2]
		case txt
			# stuff
		case md
			# stuff
		case png jpg
			# stuff
	end

```
### c
24 LOC
16 SLOC
```c
#include <stdio.h>
#include <dirent.h>
int main(void){
	struct dirent *files;
	DIR *dir = opendir(".");
	if (dir == NULL) {
		printf("Directory cannot be opened!" );
		return 0;
	}
	while ((files = readdir(dir)) != NULL) {
		char *ext = strrchr(files->d_name, '.');
		if(ext == NULL)
			continue;
		if(!strcmp('txt', ext + 1)) {
			// stuff
		} else if(!strcmp('txt', ext + 1)) {
			// stuff
		} else if(!strcmp('txt', ext + 1) || !strcmp('txt', ext + 1)) {
			// stuff
		}
	}
	closedir(dir);
	return 0;
}
```
### python
8 LOC
7 SLOC
```python
import os

for filename in os.listdir(directory):
	if filename.endswith('.txt'):
		# stuff
	elif filename.endswith('.md'):
		# stuff
	elif filename.endswith('.png') or filename.endswith('.jpg'):
		# stuff
```
### banter
11 LOC
8 SLOC
```smalltalk
Files '*' foreach [ file |
	file extension ifExists switch
		case: 'txt' do: [
			"stuff"
		];
		case: 'md' do: [
			"stuff"
		];
		case: 'png' | 'jpg' do: [
			"stuff"
		];
]
```
