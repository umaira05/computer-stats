Computer Statistics Exporter
===========================
Report Login and Logout Events from Event Viewer

# Quick start
```console
$ make btlab_stats.exe
$ ./btlab_stats.exe IN_FILENAME
```

# Input
This program parses through a .txt file and finds all the login/logout events. It then outputs it all in a csv file called output.csv which can easily be imported into Google Sheets for further data analysis. The input file should be a text file with a header of 
```text
TimeGenerated	EventID	LoggedOff	USER	LogonTYPE
```
followed by a list of login/logout events as produced by Event Viewer. An example of such an event would look something like this
```text
2022-05-18 11:32:42 4634    aumair
```
or, alternatively, something like this
```text
2022-05-18 11:21:34 4624    M-2AUGLRJWOF$   aumair  4
```
# Output
The output is a csv with entries of the format

| Time Generated | Event ID | Machine | User | Logon Type | Duration |
| ------ | ------ | ------ | ------ | ------ | ------ |
| 5/18/2022 11:21:34 AM | 4634 | M-2AUGLRJWOF$ | aumair | 4 | 11:08 |

# Logic
1. Read in file name and open input file.
2. Read every line and determine if it's a logout line. If it is, keep reading until
a. a login line is found. Then make the event.
b. a header line or logout line by the same user is found. Don't make any event. Once that is done, move the cursor back to the logout line.
3. Move on to the next line and repeat step 2 until the last line is reached.
4. Get rid of the duplicate events.
5. Output all the events into output.csv.
6. Return the amount of time the process took.

# Acknowledgements
This project was created under the discretion of William Manspeaker (<man@umich.edu>) for the University of Michigan Ann Arbor.