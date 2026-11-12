# 0249 The repository URL is deferred

Date: 2026-11-12. Source: the owner's reply to the release commit review: there is no repository URL yet; it is deferred until there is one, and the recommendations proceed.

1. **URL deferred.** Release 1.0.0 ships with the README's `FetchContent` block holding the marked placeholder `<galoot-repository-url>` and the tag `1.0.0`. The owner fills the URL when a repository exists. This departs from decision 0245 item 3, which expected the URL at the release commit, and it is stated in the release record's known friction. The documentation tests do not read the URL or the tag.
2. **rga3 wording (review finding m1).** The README's status text for the rga3 preset is reworded in the records-only tip T to say that it ships in the same tree, is below 1.0, and its interface may still change. No file carries an rga3 version; none is added for 1.0.
3. **Review minors m2 and m3** are accepted as they stand: the status sentence names the release it is in, and the placeholder is stated in the text.
