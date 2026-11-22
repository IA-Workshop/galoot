mk: readme-tag-closed-hidden ready: main 704f152c, master 2383e6df, 1.0.0 95a088ec (all as in the real repository)
=== injection: readme-tag-closed-hidden
--- $ close-out-readme-tag-closed-hidden.sh '--dry-run'
dry run: every check passed at main = 704f152c; nothing merged or tagged
exit=0
--- $ close-out-readme-tag-closed-hidden.sh 
Preparing worktree (checking out 'master')
HEAD is now at 2383e6df Release galoot core 1.0.0
Merge made by the 'ort' strategy.
done: master is at 6076ca89, tag 1.0.1 -> 6076ca89. Nothing was pushed.
To publish:   git push --atomic origin main master 1.0.0 1.0.1
Then check:   git -c credential.helper= ls-remote https://github.com/IA-Workshop/galoot.git refs/heads/master refs/tags/1.0.0 refs/tags/1.0.1
exit=0
--- clone state CHANGED:
--- /tmp/claude-1000/-home-brianm-projects-utils-math-core/20d19078-8b10-4646-bf3c-dde482e5309c/scratchpad/close-out-rehearsal-2/logs/state-readme-tag-closed-hidden-before.txt	2026-11-21 20:11:40.932784046 -0500
+++ /tmp/claude-1000/-home-brianm-projects-utils-math-core/20d19078-8b10-4646-bf3c-dde482e5309c/scratchpad/close-out-rehearsal-2/logs/state-readme-tag-closed-hidden-after.txt	2026-11-21 20:11:42.160786423 -0500
@@ -1,7 +1,8 @@
 HEAD refs/heads/main 704f152c64fd3f01e39a66640a254443f3421c0a
 refs/heads/main 704f152c64fd3f01e39a66640a254443f3421c0a
-refs/heads/master 2383e6df5e9dd9e79832a8dbb3d8af9207a5913f
+refs/heads/master 6076ca896a7486a3fbb254a4290e1180be151fc6
 refs/tags/1.0.0 95a088ec9a15d505b9e150cd103d2d726b7503b0
+refs/tags/1.0.1 b44120be2df7564131c7e9ae9f19fb16d121bbeb
 status:
 worktrees:
 worktree /tmp/claude-1000/-home-brianm-projects-utils-math-core/20d19078-8b10-4646-bf3c-dde482e5309c/scratchpad/close-out-rehearsal-2/clones/readme-tag-closed-hidden
--- release check (M = 704f152c64fd3f01e39a66640a254443f3421c0a)
master parents: 6076ca896a7486a3fbb254a4290e1180be151fc6 2383e6df5e9dd9e79832a8dbb3d8af9207a5913f 704f152c64fd3f01e39a66640a254443f3421c0a
master^1 = old master: yes
master^2 = M: yes
trees: master 3f4026e87357671c5c382b51c14bc11266331eec, M 3f4026e87357671c5c382b51c14bc11266331eec, main now 3f4026e87357671c5c382b51c14bc11266331eec
tag 1.0.1: objecttype tag, object 6076ca896a7486a3fbb254a4290e1180be151fc6 (commit), message "galoot core 1.0.1, CONVENTIONS 1.18"
merge message: Release galoot core 1.0.1
main now: 704f152c64fd3f01e39a66640a254443f3421c0a
--- the hidden edit and the release: README.md
$ git diff --quiet 704f152c 1.0.1^{tree}; echo $?
0
$ grep -c -e '^  GIT_TAG "1.0.1"$' (the tagged file, then the working tree)
1
0
$ git ls-files -v README.md (h = assume-unchanged)
h README.md
$ git diff --stat 1.0.1 -- README.md (index view; hidden)
(end)
$ git diff --no-index --stat <(git show 1.0.1:README.md) README.md
74c74
<   GIT_TAG "1.0.1"
---
>   GIT_TAG "1.0.1")
