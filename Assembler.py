##CPU ARCH Complier

import sys
f = open('program','r');
out = open("program.obj",'w+');
if( f == None):
	print( "File not opened");
	sys.exit();
str_arr = f.read().split();
final = 0;
loc = 0;
val = 0;
cmd = "";
for word in str_arr:
	if(word == "##"):
		loc = 3;
	elif( loc == 0 ):
		cmd = word;
		print(word);
		if( word == "NOP"):
			loc = 0;
			out.write(str(hex(0)));
			final = 0;
			out.write("\n");
			continue;
		elif( word == "ADD"):
			final += 0x001;
		elif( word == "SUB"):
			final += 0x002;
		elif( word == "MUL"):
			final += 0x003;
		elif( word == "DIV"):
			final += 0x004;
		elif( word == "MOD"):
			final += 0x005;
		elif( word == "AND"):
			final += 0x006;
		elif( word == "OR"):
			final += 0x007;
		elif( word == "JMP"):
			final += 0x008;
		elif( word == "ISZ"):
			final += 0x009;
		elif( word == "LDM"):
			final += 0x00A;
		elif( word == "LDI"):
			final += 0x30B;
		elif( word == "STO"):
			final += 0x30C;
		elif( word == "EXIT"):
			final += 0x00D;
		final = final << 20;
		if(cmd == "EXIT"):
			loc = 0;
			out.write(str(hex(final)));
			final = 0;
			out.write("\n");
			continue;
		loc = 1;
	elif(loc == 1):
		print(word);
		if( word == "R0"):
			val = 0 << 10;
		elif( word == "R1"):
			val = 1 << 10;
		elif( word == "R2"):
			val = 2 << 10;
		elif( word == "R3"):
			val = 3 << 10;
		else:
			val = int(word) << 10;
		if( cmd == "JMP" or cmd == "ISZ"):
			loc = 0;
			final += val;
			out.write(str(hex(final)));
			final = 0;
			out.write("\n");
		else:
			loc = 2;
	elif(loc == 2):
		print(word);
		if( word == "R0"):
			val = 0;
		elif( word == "R1"):
			val = 1;
		elif( word == "R2"):
			val = 2;
		elif( word == "R3"):
			val = 3;
		else:
			val += int(word);
		loc = 0;
		final += val;
		out.write(str(hex(final)));
		final = 0;
		out.write("\n");
	elif(loc == 3):
		loc == 0;
		val = int(word);
		print(val);
		out.write(str(hex(val)));
		out.write("\n");
print(final);

