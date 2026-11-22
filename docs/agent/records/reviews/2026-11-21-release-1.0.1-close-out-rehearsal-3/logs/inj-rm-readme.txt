mk: rm-readme ready: main e6320526, master 2383e6df, 1.0.0 95a088ec; script blob 3ea57051; copy differs in the cd and W= lines only
=== injection: rm-readme
(git replace: 578aa15b now has tree b8c68764, via cad8671d)
(git replace: 75b9a030 now has tree 3a818e1f, via eb9de367)
(R and T replaced by trees without README.md; README.md removed on main)
M (main before the runs): 91e62316a7510915529f4ae2a67ac9c7c3f6ef5d
--- $ close-out-rm-readme.sh '--dry-run'   ($# = 1)
fatal: path 'README.md' does not exist in '91e62316a7510915529f4ae2a67ac9c7c3f6ef5d'
close-out: no README.md at main
exit=1
(clone state unchanged)
--- $ close-out-rm-readme.sh    ($# = 0)
fatal: path 'README.md' does not exist in '91e62316a7510915529f4ae2a67ac9c7c3f6ef5d'
close-out: no README.md at main
exit=1
(clone state unchanged)
--- clone state unchanged by every run (refs, HEAD, status, worktrees)
