TEAM: Mysterious

160050009 : Sourabh Tote
160050050 : Akshay Patidar
160050106 : Dhruv Jaglan
160050108 : Bhavesh Dhingra



- hdlmake.py is used to compile the project

- host
- Place the main.c in "~/20140524/makestuff/apps/flcli" directory.
- Change the path of network.txt in line 1144.
- Run compile.sh to compile this

- fpga
- Place board.ucf in "~/20140524/makestuff/hdlmake/apps/makestuff/swled/templates/fx2all/boards/atlys" directory.
- Place top_level.vhdl in "~/20140524/makestuff/hdlmake/apps/makestuff/swled/templates/fx2all/vhdl" directory.
- Place harness.vhdl in "~/20140524/makestuff/hdlmake/apps/makestuff/swled/templates" directory.
- Place network.txt in home directory.
- Place all remaining files except bash scripts in "~/20140524/makestuff/hdlmake/apps/makestuff/swled/cksum/vhdl" directory.
- Run compile.sh to compile the vhdl project.
- Run program.sh to load the fpgalink firmware and program the board.
- Run run.sh to start the backend program.

- uart_relay (for optional part)
- Install pyserial for python3 on laptop acting as uart relay.
- Connect uart port of two boards to that laptop.
- Run the uart_relay.py with passing the port names as arguments.
- Board to board communication method is as described in problem statement, i.e. using left right push buttons and switches to indicate change in certain values for a direction.