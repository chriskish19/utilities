# Directory Tool (dirt)

## Info:
<p> 
Sync directories together and have files automatically updated/copied from source directory to destination directory. 
Use command line arguments or a text file to specify arguments and paths.
</p>

## Example:

```
// script example code, this is a comment

args : -copy -watch -mirror
{
	src ./test
	dst ./hello
}
```
<p>
Its a simple syntax parser that looks for right token and in the right place. If there is a syntax error the terminal will 
output a syntax error. But only a general error it doesnt tell what token is missing or where. There is no limit to the amount of directories
to be monitored/synced but each will use a 4MB buffer so keep that in mind. The file can be named anything but the extension type must support UTF text.
if there is spaces in the path wrap it in double quotes "./hello world folder".
</p>

## Arguments:
<li>-copy</li>
<p>
Copies files from source to destination.
</p>

<li>-watch</li>
<p>
Monitors source directory for changes and then produces those same changes in the destination directory. 
</p>

<li>-mirror</li>
<p>
Mirrors source directory changes to destination directory.
</p>

<li>src</li>
<p>
Source directory. When inputing into the command line the next argument must be the source path.
</p>

<li>dst</li>
<p>
Destination directory. When inputing into the command line the next argument must be the destination path.
</p>

<li>-new_files_only</li>
<p>
Only new files added to the source directory are added to the destination directory. Existing files are ignored.  
</p>

<li>-no_deletes</li>
<p>
If files are deleted in the source directory, they are not deleted in destination directory.
</p>

<li>-dirt_list_path</li>
<p>
The file path to the script, when inputing on the command line the next argument must be the file path.
</p>

<li>-recursive</li>
<p>
Copies directories recursively. Everything inside a directory and inside other directories within.
</p>

## Valid argument combinations:
| -copy | -watch | -mirror | -new_files_only | -no_deletes | -recursive | src | dst | -dirt_list_path |
|-------|--------|---------|-----------------|-------------|------------|-----|-----|-----------------|
|       |   ✔   |         |        ✔        |      ✔     |            | ✔  | ✔  |     ✔        |
|   ✔  |   ✔    |    ✔   |                 |             |     ✔      | ✔  | ✔  |      ✔       |
|       |   ✔    |    ✔    |                 |            |             | ✔  | ✔  |      ✔       |
|   ✔   |        |         |                 |             |     ✔      | ✔   |✔  |       ✔       |

<p>
In the valid combinations table, at most arguments are shown but arguments on each line can be less and will 
be valid as long as copy or watch is specified. Src, dst must always be specified.
</p>
