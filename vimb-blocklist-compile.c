/*
 * vimb-blocklist-compile — compile a WKContentRuleList JSON file into a
 * WebKitUserContentFilterStore so vimb (and any other WebKit consumer)
 * can attach it via webkit_user_content_manager_add_filter().
 *
 * Usage: vimb-blocklist-compile <store-path> <identifier> <json-file>
 *
 * Exit codes:
 *   0  success
 *   1  compile failed (WebKit rejected the rules)
 *   2  bad arguments or IO error
 *   3  store initialization failed
 */

#include <webkit/webkit.h>
#include <glib.h>
#include <gio/gio.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
	GMainLoop *loop;
	int exit_code;
	const char *identifier;
} save_ctx_t;

static void on_saved(GObject *source, GAsyncResult *res, gpointer data) {
	save_ctx_t *ctx = data;
	WebKitUserContentFilterStore *store = WEBKIT_USER_CONTENT_FILTER_STORE(source);
	GError *err = NULL;
	WebKitUserContentFilter *filter =
		webkit_user_content_filter_store_save_from_file_finish(store, res, &err);
	if (filter) {
		webkit_user_content_filter_unref(filter);
		ctx->exit_code = 0;
	} else {
		fprintf(stderr, "vimb-blocklist-compile: '%s' rejected: %s\n",
			ctx->identifier,
			err ? err->message : "unknown error");
		if (err) g_error_free(err);
		ctx->exit_code = 1;
	}
	g_main_loop_quit(ctx->loop);
}

int main(int argc, char **argv) {
	if (argc != 4) {
		fprintf(stderr,
			"usage: %s <store-path> <identifier> <json-file>\n",
			argv[0]);
		return 2;
	}
	const char *store_path = argv[1];
	const char *identifier = argv[2];
	const char *json_path  = argv[3];

	if (g_mkdir_with_parents(store_path, 0755) < 0) {
		fprintf(stderr,
			"vimb-blocklist-compile: cannot create store dir %s\n",
			store_path);
		return 2;
	}

	WebKitUserContentFilterStore *store =
		webkit_user_content_filter_store_new(store_path);
	if (!store) {
		fprintf(stderr,
			"vimb-blocklist-compile: cannot create filter store at %s\n",
			store_path);
		return 3;
	}

	GFile *json_file = g_file_new_for_path(json_path);

	save_ctx_t ctx = {
		.loop = g_main_loop_new(NULL, FALSE),
		.exit_code = 1,
		.identifier = identifier,
	};

	webkit_user_content_filter_store_save_from_file(
		store, identifier, json_file, NULL, on_saved, &ctx);

	g_main_loop_run(ctx.loop);

	g_main_loop_unref(ctx.loop);
	g_object_unref(json_file);
	g_object_unref(store);

	return ctx.exit_code;
}
