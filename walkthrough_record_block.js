(function($) {
  "use strict";

  Drupal.behaviors.walkhubRecord = {
    attach: function (context, settings) {
      var url_re = /^https?:\/\/[a-zA-Z0-9\-\.]+\.[a-zA-Z]+(\/\S*)?$/;
      var DEFAULT_URL = "http://pronovix.com/contact-us";

      $("#record-a-walkthrough-button", context).click(function (e) {
        var $url_field = $("#walkthrough-record-url", context);
        $(this).hide();
        $("#record-a-walkthrough-popup-modal", context).show();
        if ($url_field.val() === "") {
          $url_field.val(DEFAULT_URL);
          $("#url-error", context).hide();
        }
        $url_field.select();
      });

      $("#walkthrough-record-url", context).keyup(function(e) {
        if (e.keyCode === 13) {
          $("#walkthrough-start-recording", context).click();
        }
      });

      $("#walkthrough-start-recording", context).click(function (e) {
        var url = $("#walkthrough-record-url", context).val();
        if (!url_re.test(url)) {
          $("#url-error", context).show();
        } else {
          $("#url-error", context).hide();
          $("#edit-url", context).val(url);
          $(".walkhub-record-form .edit-record").click();

          var $title = $("#walkthrough-record-title-wrapper", context);
          var $severity = $("#walkthrough-record-severity-wrapper", context);
          $title.find("label").text($(".form-item-title label", context).text());
          var title_input = $("#edit-title", context).clone();
          title_input.attr("id", "walkthrough-record-title");
          $title.find(".form-element").html(title_input);
          $title.find("label").attr("for", "walkthrough-record-title");

          $severity.find("label").text($(".form-item-severity label", context).text());
          var severity_select = $("#edit-severity", context).clone();
          severity_select.attr("id", "walkthrough-record-severity");
          $severity.find(".form-element").html(severity_select);
          $severity.find("label").attr("for", "walkthrough-record-severity");

          $("#walkthrough-record-first-step", context).hide();
          $("#walkthrough-record-second-step", context).show();
          $("#walkthrough-recorder-popup-notice", context).hide();
          $(".walkthrough-record-first-step-title", context).hide();
          $(".walkthrough-record-second-step-title", context).show();
          $("#record-a-walkthrough-button .record-action", context).hide();
          $("#record-a-walkthrough-button .save-action", context).show();
        }
      });

      $("#steps", context).bind("DOMSubtreeModified", function(e) {
        var $stepsBox = $("#walkthrough-recorded-steps", context);
        var $steps = $("#steps", context);
        if (e.target.innerHTML.length > 0) {
          $stepsBox.val($steps.text().replace(/\)/g,")\n"));
        }
      });

      $("#walkthrough-record-save", context).click(function() {
        var $input_title = $("#walkthrough-record-title", context);
        var $select_severity = $("#walkthrough-record-severity", context);
        var error = false;
        if ($input_title.val() === "") {
          error = true;
          $("#walkthrough-record-title-error", context).show();
        } else {
          $("#walkthrough-record-title-error", context).hide();
        }
        if ($select_severity.val() === "") {
          error = true;
          $("#walkthrough-record-severity-error", context).show();
        } else {
          $("#walkthrough-record-severity-error", context).hide();
        }
        if (!error) {
          $("#edit-title", context).val($input_title.val());
          $("#edit-severity", context).val($select_severity.val());
          $("#edit-save", context).click();
        }
      });

      $("#walkthrough-record-reset", context).click(function() {
        $.cookie("wtRecPopup", true);
        window.location.reload();
      });

      $("#record-a-walkthrough-popup .close-popup, #record-a-walkthrough-popup-modal").click(function (e) {
        e.preventDefault();
        $("#record-a-walkthrough-popup-modal", context).hide();
        $("#record-a-walkthrough-button", context).show();
      });

      $("#record-a-walkthrough-popup", context).click(function (e) {
        e.stopPropagation();
      });

      if ($.cookie("wtRecPopup")) {
        $.cookie("wtRecPopup", null);
        $("#record-a-walkthrough-button", context).click();
      }
    }
  };
}(jQuery));
