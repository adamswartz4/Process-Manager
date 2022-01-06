Program PMan (Process Manager) launches processes and runs them in the background. You can run this program with the specifications below

Step 1: Compile all test files that will be used
Step 2: Run the command "make" to compile PMan
Step 3: Run PMan with command "./PMan"
Step 4: Input using the available commands

Supported commands of PMan:
1. bg <cmd>: start program <cmd> as a background process
2. bglist: display a list of all the programs currently executing in the background and how many are running currently
3. bgkill <pid>: kills a process <pid>
4. bgstop <pid>: temporarily stops a process <pid>
5. bgstart <pid>: begins a process <pid> after it has been stopped
6. pstat <pid>: Display information comm, state, utime, stime, rss, voluntary_ctxt_switches and nonvoluntary_ctxt_switches of process <pid>
