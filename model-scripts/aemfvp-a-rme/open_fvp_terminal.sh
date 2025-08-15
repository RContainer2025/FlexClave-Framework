#!/bin/bash

# TERMINAL_TITLE="$1"
# TERMINAL_PORT="$2"

# zellij action new-pane -n "$TERMINAL_TITLE" --floating -c -- bash -c "telnet localhost $TERMINAL_PORT"
zellij action new-pane -n %title --floating -c -- bash -c "telnet localhost %port