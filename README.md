# vimb-blocklist

WebKit-native ad + tracker + cookie-banner blocking for [vimb](https://fanglingsu.github.io/vimb/) and other [WebKitGTK 6.0](https://webkitgtk.org/) applications on Linux.

Two artifacts:

- **`vimb-blocklist-compile`** — a small C binary that compiles a WKContentRuleList JSON file into a `WebKitUserContentFilterStore` under a given identifier. Used by the refresh cron, and can be invoked manually.
- **`/etc/cron.weekly/vimb-blocklist-refresh`** — bash script driving the full pipeline: fetch three EasyList sources (EasyList, EasyPrivacy, EasyList Cookie List), convert to WKContentRuleList JSON via [`adblock-rust-cli`](https://github.com/craig-miller/adblock-rust-cli), compile via `vimb-blocklist-compile`, atomic swap into `/var/cache/vimb-blocklist/store/`.

Vimb (patched with per-package content-filter support in [gentoo-overlay](https://github.com/craig-miller/gentoo-overlay)) reads from the shared store at startup and attaches all three filters to every WebView.

## Directory layout

- `/var/lib/vimb-blocklist/*.txt` — the fetched source lists.
- `/var/lib/vimb-blocklist/*.json` — the converted WKContentRuleList JSON.
- `/var/cache/vimb-blocklist/store/` — the compiled bytecode cache. Owned by root, world-readable.

## Manual invocation

```
vimb-blocklist-compile /var/cache/vimb-blocklist/store easylist /var/lib/vimb-blocklist/easylist.json
```

## Trigger a refresh

```
sudo /etc/cron.weekly/vimb-blocklist-refresh
```

## License

MIT.
