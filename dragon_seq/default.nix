with import <nixpkgs> {};

stdenv.mkDerivation rec {
  name = "dragon_seq-${version}";
  version = "1.0";

	src = ./.;
	
	buildInputs = [ gcc cairo pkg-config ];
	
	installPhase = ''
		mkdir -p $out/bin
		make
		cp dragon $out/bin
	'';
}
