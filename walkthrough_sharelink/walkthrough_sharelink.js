(function ($) {
  "use strict";

  Drupal.behaviors.walkthrough_sharelink = {
    attach: function (context) {
      $("input[name=\"walkthrough-private-public\"]:not(.processed)", context)
        .addClass("processed")
        .each(function () {
          var status = $(this).val();
          var token = $(this).data("token");
          var nid = $(this).data("nid");

          var element_settings = {
            progress: {
              type: "throbber"
            },
            url: Drupal.settings.basePath + (status ? "walkthrough/publish/nojs/" : "walkthrough/unpublish/nojs/") + nid + "?token=" + token,
            // Intentionally using "click" instead of "change", because it has better browser support.
            event: "click"
          };

          var base = $(this).attr("id");

          Drupal.ajax[base] = new Drupal.ajax(base, this, element_settings);
        });
    }
  };
})(jQuery);
