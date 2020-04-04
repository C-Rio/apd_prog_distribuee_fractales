with import <nixpkgs> {};

stdenv.mkDerivation rec {
  name = "flocon_mpi-${version}";
  version = "1.0";

	src = ./.;
	
	buildInputs = [ gcc cairo pkg-config openmpi];
	
	installPhase = ''
		mkdir -p $out/bin
		make
		cp flocon_mpi $out/bin
	'';
}
