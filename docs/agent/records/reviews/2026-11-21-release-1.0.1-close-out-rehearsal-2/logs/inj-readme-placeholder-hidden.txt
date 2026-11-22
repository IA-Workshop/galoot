mk: readme-placeholder-hidden ready: main 704f152c, master 2383e6df, 1.0.0 95a088ec (all as in the real repository)
=== injection: readme-placeholder-hidden
--- $ close-out-readme-placeholder-hidden.sh '--dry-run'
dry run: every check passed at main = 704f152c; nothing merged or tagged
exit=0
--- $ close-out-readme-placeholder-hidden.sh 
Preparing worktree (checking out 'master')
HEAD is now at 2383e6df Release galoot core 1.0.0
Merge made by the 'ort' strategy.
done: master is at be2cf9bc, tag 1.0.1 -> be2cf9bc. Nothing was pushed.
To publish:   git push --atomic origin main master 1.0.0 1.0.1
Then check:   git -c credential.helper= ls-remote https://github.com/IA-Workshop/galoot.git refs/heads/master refs/tags/1.0.0 refs/tags/1.0.1
exit=0
--- clone state CHANGED:
--- /tmp/claude-1000/-home-brianm-projects-utils-math-core/20d19078-8b10-4646-bf3c-dde482e5309c/scratchpad/close-out-rehearsal-2/logs/state-readme-placeholder-hidden-before.txt	2026-11-21 20:11:43.010457533 -0500
+++ /tmp/claude-1000/-home-brianm-projects-utils-math-core/20d19078-8b10-4646-bf3c-dde482e5309c/scratchpad/close-out-rehearsal-2/logs/state-readme-placeholder-hidden-after.txt	2026-11-21 20:11:44.175790324 -0500
@@ -1,7 +1,8 @@
 HEAD refs/heads/main 704f152c64fd3f01e39a66640a254443f3421c0a
 refs/heads/main 704f152c64fd3f01e39a66640a254443f3421c0a
-refs/heads/master 2383e6df5e9dd9e79832a8dbb3d8af9207a5913f
+refs/heads/master be2cf9bc22491c01d194ff8946ca4412cd402bd2
 refs/tags/1.0.0 95a088ec9a15d505b9e150cd103d2d726b7503b0
+refs/tags/1.0.1 8e843c720f35bd371f0a945af5cc8b5d6bd12828
 status:
 worktrees:
 worktree /tmp/claude-1000/-home-brianm-projects-utils-math-core/20d19078-8b10-4646-bf3c-dde482e5309c/scratchpad/close-out-rehearsal-2/clones/readme-placeholder-hidden
--- release check (M = 704f152c64fd3f01e39a66640a254443f3421c0a)
master parents: be2cf9bc22491c01d194ff8946ca4412cd402bd2 2383e6df5e9dd9e79832a8dbb3d8af9207a5913f 704f152c64fd3f01e39a66640a254443f3421c0a
master^1 = old master: yes
master^2 = M: yes
trees: master 3f4026e87357671c5c382b51c14bc11266331eec, M 3f4026e87357671c5c382b51c14bc11266331eec, main now 3f4026e87357671c5c382b51c14bc11266331eec
tag 1.0.1: objecttype tag, object be2cf9bc22491c01d194ff8946ca4412cd402bd2 (commit), message "galoot core 1.0.1, CONVENTIONS 1.18"
merge message: Release galoot core 1.0.1
main now: 704f152c64fd3f01e39a66640a254443f3421c0a
--- the hidden edit and the release: README.md
$ git diff --quiet 704f152c 1.0.1^{tree}; echo $?
0
$ grep -c -e '<galoot-repository-url>' (the tagged file, then the working tree)
0
1
$ git ls-files -v README.md (h = assume-unchanged)
h README.md
$ git diff --stat 1.0.1 -- README.md (index view; hidden)
(end)
$ git diff --no-index --stat <(git show 1.0.1:README.md) README.md
257a258
> <galoot-repository-url>
