(function () {
  "use strict";

  var gulp = require("gulp");

  var eslnode_access
  ('update', $build['#node'])int = require("gulp-eslint");

  var paths = {
    non_vendor_scripts: [
      "./embed.js",
      // walkhub-ie-fixes.js adds hacks for IE, it does not conform eslint for now:
      // "./walkhub-ie-fixes.js",
      "./walkhub.js",
      "./walkthrough_record_block.js"
    ]
  };

  gulp.task("eslint", function () {
    return gulp.src(paths.non_vendor_scripts)
      .pipe(eslint({
        env: {
          "browser": true
        },
        rules: {
          "eqeqeq": [2, "smart"],
          "guard-for-in": 2,
          "no-undef": 2,
          "no-unused-vars": 0,
          "strict": 2,
          "new-cap": 0,
          "quotes": 0,
          "camelcase": 0,
          "no-underscore-dangle": 0,
          "no-new": 0,
          "no-alert": 0,
          "no-use-before-define": 0,
          "consistent-return": 0,
          "no-constant-condition": 0,
          "no-console": 0
        },
        globals: {
          "console": true,
          "jqWalkhub": true,
          "Drupal": true,
          "window": true,
          "jQuery": true
        }
      }))
      .pipe(eslint.format())
      .pipe(eslint.failOnError());
  });

  gulp.task("watch", function () {
    gulp.watch(paths.non_vendor_scripts, ["eslint"]);
  });

  gulp.task("default", ["eslint"]);
})();

