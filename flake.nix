{
  description = "cpp env";

  inputs.flake-utils.url = "github:numtide/flake-utils";

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem
      (system:
        let pkgs = nixpkgs.legacyPackages.${system}; in
        {
          devShells.default = pkgs.mkShell {
            buildInputs = with pkgs; [
              clang-tools
              llvmPackages.libcxxClang
              valgrind
              gcc
              gnumake
              ninja
              cmake
              gdb
              lldb
              doctest
              binutils
            ];
          };
        }
      );
}
