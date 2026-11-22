mk: readme-tag-committed ready: main 704f152c, master 2383e6df, 1.0.0 95a088ec (all as in the real repository)
=== injection: readme-tag-committed
--- $ close-out-readme-tag-committed.sh '--dry-run'
close-out: non-records change after R
exit=1
--- $ close-out-readme-tag-committed.sh 
close-out: non-records change after R
exit=1
--- clone state unchanged by both runs (refs, HEAD, status, worktrees)
