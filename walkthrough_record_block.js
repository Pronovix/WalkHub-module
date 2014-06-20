(function($) {
  "use strict";
  $(document).ready(function() {

    var url_re = /^https?:\/\/[a-zA-Z0-9\-\.]+\.[a-zA-Z]+(\/\S*)?$/;
    var DEFAULT_URL = "http://pronovix.com/contact-us";

    $("#record-a-walkthrough-button").click(function (e) {
      var $url_field = $("#walkthrough-record-url");
      $(this).hide();
      $("#record-a-walkthrough-popup-modal").show();
      if ($url_field.val() === "") {
        $url_field.val(DEFAULT_URL);
        $("#url-error").hide();
      }
      $url_field.select();
    });

    $("#walkthrough-record-url").keyup(function(e) {
      if (e.keyCode === 13) {
        $("#walkthrough-start-recording").click();
      }
    });

    $("#walkthrough-start-recording").click(function (e) {
      var url = $("#walkthrough-record-url").val();
      if (!url_re.test(url)) {
        $("#url-error").show();
      } else {
        $("#url-error").hide();
        $("#edit-url").val(url);
        $("#edit-record").click();

        var $title = $("#walkthrough-record-title-wrapper");
        var $severity = $("#walkthrough-record-severity-wrapper");
        $title.find("label").text($(".form-item-title label").text());
        var title_input = $("#edit-title").clone();
        title_input.attr("id","walkthrough-record-title");
        $title.find(".form-element").html(title_input);
        $title.find("label").attr("for", "walkthrough-record-title");

        $severity.find("label").text($(".form-item-severity label").text());
        var severity_select = $("#edit-severity").clone();
        severity_select.attr("id","walkthrough-record-severity");
        $severity.find(".form-element").html(severity_select);
        $severity.find("label").attr("for", "walkthrough-record-severity");

        $("#walkthrough-record-first-step").hide();
        $("#walkthrough-record-second-step").show();
        $("#walkthrough-recorder-popup-notice").hide();
        $(".walkthrough-record-first-step-title").hide();
        $(".walkthrough-record-second-step-title").show();
        $("#record-a-walkthrough-button .record-action").hide();
        $("#record-a-walkthrough-button .save-action").show();
      }
    });

    $("#steps").bind('DOMSubtreeModified', function(e) {
      var $stepsBox = $("#walkthrough-recorded-steps");
      var $steps = $("#steps");
      if (e.target.innerHTML.length > 0) {
        $stepsBox.val($steps.text().replace(/\)/g,")\n"));
      }
    });

    $("#walkthrough-record-save").click(function() {
      var $input_title = $("#walkthrough-record-title");
      var $select_severity = $("#walkthrough-record-severity");
      var error = false;
      if ($input_title.val() === "") {
        error = true;
        $("#walkthrough-record-title-error").show();
      } else {
        $("#walkthrough-record-title-error").hide();
      }
      if ($select_severity.val() === "") {
        error = true;
        $("#walkthrough-record-severity-error").show();
      } else {
        $("#walkthrough-record-severity-error").hide();
      }
      if (!error) {
        $("#edit-title").val($input_title.val());
        $("#edit-severity").val($select_severity.val());
        $("#edit-save").click();
      }
    });

    $("#walkthrough-record-reset").click(function() {
      $.cookie("wtRecPopup", true);
      window.location.reload();
    });

    $("#record-a-walkthrough-popup .close-popup, #record-a-walkthrough-popup-modal").click(function (e) {
      e.preventDefault();
      $("#record-a-walkthrough-popup-modal").hide();
      $("#record-a-walkthrough-button").show();
    });

    $("#record-a-walkthrough-popup").click(function (e) {
      e.stopPropagation();
    });

    if ($.cookie("wtRecPopup")) {
      $.cookie("wtRecPopup", null);
      $("#record-a-walkthrough-button").click();
    }

  });

}(jQuery));
