split-window -h R --interactive
send-keys 'source("receiver.R", echo=T)' Enter
select-pane -t0; send-keys 'R --interactive && tmux kill-session' Enter 'source("sender.R", echo=T)' Enter
