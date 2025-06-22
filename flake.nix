{
  inputs = {
    utils.url = "github:numtide/flake-utils";
  };
  outputs =
    {
      nixpkgs,
      utils,
      ...
    }:
    utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        orcv = pkgs.rPackages.buildRPackage {
          name = "orcv";
          version = "1.1";
          src = ./.;
        };

        prod_pkgs = [ orcv ];
        dev_pkgs = prod_pkgs ++ [ pkgs.rPackages.languageserver ];

        R_dev = pkgs.rWrapper.override { packages = dev_pkgs; };
        radian_dev = pkgs.radianWrapper.override { packages = dev_pkgs; };
        radian_dev_exec = pkgs.writeShellApplication {
          name = "r";
          runtimeInputs = [ radian_dev ];
          text = "exec radian";
        };

        tex = pkgs.texlive.combine {
          inherit (pkgs.texlive) scheme-medium inconsolata;
        };

        bundle = pkgs.stdenv.mkDerivation {
          name = "orcv-bundle";
          nativeBuildInputs = [ R_dev ];
          src = ./.;
          buildPhase = ''
            mkdir -p $out
            R CMD build .
            cp *.tar.gz $out/
          '';
          installPhase = "true";
        };
        check =
          pkgs.runCommand "check"
            {
              nativeBuildInputs = [
                R_dev
                tex
                pkgs.html-tidy
              ];
              buildInputs = [ bundle ];
            }
            ''
              mkdir -p $out
              R CMD check --as-cran ${bundle}/*.tar.gz
              check_success=$?
              cp -r *.Rcheck $out
              exit $?
            '';

        genCompileCommands = pkgs.writeShellScript "gen-compile-commands" ''
          mkdir -p src
          R_INCLUDE=$(R RHOME)/include
          CFLAGS=$(R CMD config CFLAGS)
          echo "[" > compile_commands.json
          first=1
          for file in src/*.c; do
            cmd="clang -c -I$R_INCLUDE $CFLAGS $file"
            entry="{\"directory\": \"$(pwd)\", \"file\": \"$file\", \"command\": \"$cmd\"}"
            if [ $first -eq 1 ]; then
              echo "  $entry" >> compile_commands.json
              first=0
            else
              echo "  ,$entry" >> compile_commands.json
            fi
          done
          echo "]" >> compile_commands.json
        '';

      in
      {
        packages = {
          default = orcv;
          bundle = bundle;
        };
        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [
            clang-tools
            R_dev
            radian_dev_exec
            tex
          ];
          shellHook = "${genCompileCommands}";
        };
        checks.default = check;
      }
    );
}
