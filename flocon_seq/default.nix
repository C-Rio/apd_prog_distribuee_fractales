with import <nixpkgs> {};

stdenv.mkDerivation rec {
  name = "flocon_seq-${version}";
  version = "1.0";

	src = ./.;
	
	buildInputs = [ gcc cairo pkg-config ];
	
	installPhase = ''
		mkdir -p $out/bin
		make
		cp flocon_seq $out/bin
	'';
}
