sed -i \
    -e '/"-lm"/d' \
    -e '/"-lcunit"/d' \
compile_commands.json
