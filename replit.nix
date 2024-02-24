{ pkgs }: {
	deps = [
		pkgs.clang_12
		pkgs.ccls
		pkgs.gdb
		pkgs.gnumake
		pkgs.readline
		pkgs.valgrind
	];
	buildInputs = [ pkgs.readline pkgs.valgrind ]; 
	buildFlags = [
		"-lreadline"
	];
}