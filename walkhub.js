(function ($) {
  "use strict";

  var getdata = window.location.search.substr(1).split("&").reduce(function (obj, str) {
    var arrstr = str.split("=");
    obj[arrstr.shift()] = arrstr.join("=");
    return obj;
  }, {});
  var MAXIMUM_ZINDEX = 2147483647;
  var walkthroughOrigin;
  var csrf_token = null;

  // @TODO convert these into proper objects. Remove the singleton state of methods.*.object.
  var methods = {
    iframe: {
      name: "iFrame",
      linkcheck: false,
      execute: function (url, recording) {
        var widget,
          iframe = $("<iframe />")
            .attr("src", url)
            .attr("frameborder", 0)
            .attr("scrolling", "auto")
            .attr("allowtransparency", "true");

        methods.iframe.object = iframe;

        var widgetHeader = $("<div />");
        var recordingContainer = $("<div />")
          .addClass("recordingContainer");
        $("<span/>")
          .attr("id", "recDot")
          .appendTo(recordingContainer);
        $("<span />")
          .addClass("recLabel")
          .text(Drupal.t("recording"))
          .appendTo(recordingContainer);
        recordingContainer.appendTo(widgetHeader);

        var recordedSteps = $("<span />")
          .attr("id", "stepListContainer");
        $("<ol />")
          .attr("id", "stepList")
          .appendTo(recordedSteps);
        $("<span />")
          .addClass("icon-chevron-down")
          .appendTo(recordedSteps);
        recordedSteps.appendTo(widgetHeader);

        $("<button />")
          .addClass("cta")
          .attr("id", "stopRecording")
          .html(Drupal.t("Finish & save"))
          .appendTo(widgetHeader);

        iframe
          .appendTo($("body"))
          .dialog({
            modal: true,
            autoOpen: true,
            draggable: false,
            resizable: false,
            beforeClose: function(event, ui) {
              $("body").css({
                overflow: "inherit"
              });
            },
            create: function (event, ui) {
              $("body").css({
                overflow: "hidden"
              });
              if (recording) {
                $(this).siblings().find("span.ui-dialog-title").html(widgetHeader.html());
                $(this).siblings("div.ui-dialog-titlebar").addClass("recording");
              }
            }
          });

        widget = iframe.dialog("widget");

        $("#stepList").css({
          "padding-left": ($(".stepsLabel", "span.ui-dialog-title").width() + 5) + "px"
        });

        function blinkRecDot() {
          var $recDot = $("#recDot");
          if ($recDot.is("visible")) {
            setTimeout(blinkRecDot, 900);
          } else {
            setTimeout(blinkRecDot, 600);
          }
          $recDot.toggle();
        }

        blinkRecDot();

        function resize() {
          var width = $(window).width();
          var height = $(window).height();
          var $title = $("span.ui-dialog-title");
          var $stepList = $("#stepList");

          // If full window is required.
          if ($("body").hasClass("walkthrough-full-window")) {
            width = $(window).width();
            height = $(window).height();
            // Hide dialog title.
            $(".ui-dialog-titlebar", widget).hide();
            // Make the dialog display in full window.
            widget.css("top", "0px");
            widget.css("bottom", "0px");
            widget.css("left", "0px");
            widget.css("right", "0px");
          }

          iframe.dialog("option", "width", width);
          iframe.dialog("option", "height", height);
          iframe.dialog("option", "position", "top");

          widget.css("padding", "0px");
          widget.css("margin", "0px");

          iframe.css("width", width);
          iframe.css("height", height - $title.parent().innerHeight() - 2);
          iframe.css("position", "center");

          var stepListWidth = $title.width() - $title.find("button.cta").innerWidth() - $title.find("#recDot").parent().innerWidth() - 50;
          $stepList.innerWidth(stepListWidth);
          $stepList.parent().find("span.icon-chevron-down").css("left", (stepListWidth - 22) + "px");
        }

        resize();

        // <IE8 uses window.attachEvent() and not window.addEventListender().
        if (window.addEventListener) {
          window.addEventListener("resize", resize);
        } else {
          window.attachEvent("onresize", resize);
        }

        iframe
          .parent()
          .css("z-index", MAXIMUM_ZINDEX);

        if (getdata.embedorigin) {
          setTimeout(function () {
            $(".ui-dialog-titlebar-close").click(function () {
              embeddedPost({type: "end"});
            });
          }, 500);
        }

        return iframe.get(0).contentWindow;
      },
      teardown: function () {
        if (methods.iframe.object) {
          methods.iframe.object.dialog("close");
          methods.iframe.object.remove();
        }
      },
      valid: true
    }
  };


  function jqCompat(version) {
    var jqversionparts = $.fn.jquery.split(".");
    var versionparts = version.split(".");

    for (var p in versionparts) {
      if (versionparts.hasOwnProperty(p)) {
        if (versionparts[p] > (jqversionparts[p] || 0)) {
          return false;
        }
        if (versionparts[p] < (jqversionparts[p] || 0)) {
          return true;
        }
      }
    }

    return true;
  }

  function getDefaultParameters(walkthroughlink) {
    var parameters = {};
    var data = walkthroughlink.data();
    var wtParamPrefix = jqCompat("1.6") ? "walkthroughParameter" : "walkthrough-parameter-";

    for (var k in data) {
      if (data.hasOwnProperty(k) && k.indexOf(wtParamPrefix) === 0) {
        var parameter = k.substr(wtParamPrefix.length).toLowerCase();
        var default_value = data[k];
        parameters[parameter] = getdata[parameter] || default_value;
      }
    }

    return parameters;
  }

  function getPrerequisites(walkthroughlink) {
    var prereqs = {};
    var attrs = walkthroughlink.get(0).attributes;
    for (var i in attrs) {
      if (attrs.hasOwnProperty(i)) {
        var attr = attrs[i];
        if (attr.name) {
          var match = attr.name.match(/^data-walkthrough-prerequsite-[\d]+-([0-9a-f-]+)$/i);
          if (match) {
            prereqs[match[1]] = attr.value;
          }
        }
      }
    }

    return prereqs;
  }


  function createDialogForm(walkthroughlink, server, state) {
    var form = $('<form />');
    $('<fieldset />')
      .addClass('walkthrough-settings')
      .appendTo(form);
    $('<a />')
      .addClass('walkthrough-more-toggle')
      .attr('href', '#')
      .text(Drupal.t('More'))
      .appendTo(form);
    $('<fieldset />')
      .addClass('walkthrough-more')
      .hide()
      .appendTo(form);

    $(form).find('.walkthrough-more-toggle').click(function() {
      var more_options = $(form).find('.walkthrough-more');
      more_options.toggle();
      if (more_options.is(':visible')) {
        $(this).text(Drupal.t('Less'));
      } else {
        $(this).text(Drupal.t('More'));
      }
    });

    var parameters = getDefaultParameters(walkthroughlink);
    var dialog = $("<div />")
      .attr("id", "walkthrough-dialog-" + Math.random().toString())
      .attr("title", Drupal.t("Start Walkthrough"))
      .addClass("walkthrough-dialog")
      .hide()
      .append($(form));
    var fieldset = dialog.find("fieldset.walkthrough-more");
    var buttons = {};

    // Severity of changes.
    if (walkthroughlink.data("walkthrough-severity") !== 'undefined') {
      $("<div />")
        .addClass("wt-severity-" + walkthroughlink.data("walkthrough-severity"))
        .html(walkthroughlink.data("walkthrough-severity-text"))
        .appendTo(dialog.find("fieldset.walkthrough-settings"));
    }

    // Use proxy.
    var useproxy = null;
    var httpproxy = !!walkthroughlink.attr("data-walkthrough-proxy-url");
    if (httpproxy) {
      useproxy = $("<input />")
        .attr("type", "checkbox")
        .attr("name", "useproxy")
        .attr("id", "useproxy")
        .appendTo(dialog.find("fieldset.walkthrough-settings"));
      $("<label />")
        .attr("for", "useproxy")
        .html(Drupal.t("Use proxy"))
        .appendTo(dialog.find("fieldset.walkthrough-settings"));
      $('<br />')
        .appendTo(dialog.find("fieldset.walkthrough-settings"));

      if (getdata.useproxy !== "0") {
        useproxy.attr("checked", "checked");
      }
    }

    // Disable clicking outside.
    var disableClicks = $("<input />")
      .attr("type", "checkbox")
      .attr("name", "disableclicks")
      .attr("id", "disableclicks")
      .attr("checked", "checked")
      .appendTo(dialog.find("fieldset.walkthrough-settings"));
    $("<label />")
      .attr("for", "disableclicks")
      .html(Drupal.t("Disable clicks outside of the walkthrough"))
      .appendTo(dialog.find("fieldset.walkthrough-settings"));

    // Prerequisites.
    var prerequisites = getPrerequisites(walkthroughlink);
    if (prerequisites && !$.isEmptyObject(prerequisites)) {
      $("<p />")
        .html(Drupal.t("Before this Walkthrough you may need to run:"))
        .appendTo(fieldset);
      for (var prerequsite in prerequisites) {
        if (prerequisites.hasOwnProperty(prerequsite)) {
          $("<label />")
            .appendTo(fieldset)
            .html(prerequisites[prerequsite])
            .prepend($("<input />")
              .attr("type", "checkbox")
              .attr("name", "prereq-" + prerequsite)
              .attr("id", "prereq-" + prerequsite)
              .attr("value", prerequsite)
              .addClass("prerequisite")
            );
        }
      }

      // If wt_play_prerequisites GET parameter is passed we automatically check
      // all the prerequisites to play.
      if ($("body").hasClass("walkthrough-play-prerequisites")) {
        $("input[type=checkbox].prerequisite", dialog).attr("checked","checked");
      }
    }

    // Parameters.
    $("<p />")
      .html(Drupal.t("The following parameters are available in this walkthrough:"))
      .appendTo(fieldset);

    for (var parameter in parameters) {
      if (parameters.hasOwnProperty(parameter)) {
        $("<label/>")
          .attr("for", parameter)
          .html(parameter)
          .appendTo(fieldset);
        $("<input />")
          .attr("type", "text")
          .attr("name", parameter)
          .attr("value", parameters[parameter])
          .attr("id", parameter)
          .addClass("text")
          .addClass("ui-widget-content")
          .addClass("ui-corner-all")
          .appendTo(fieldset);
      }
    }

    // Share widgets.
    $("<label />")
      .attr("for", "sharelink")
      .html(Drupal.t("Share with these parameters: "))
      .appendTo(fieldset);

    var share = $("<textarea />")
      .attr("name", "sharelink")
      .attr("readonly", "readonly")
      .addClass("share")
      .appendTo(fieldset);

    $("<label />")
      .attr("for", "embedlink")
      .html(Drupal.t("Embed with these parameters: "))
      .appendTo(fieldset);

    var embed = $("<textarea />")
      .attr("name", "embedlink")
      .attr("readonly", "readonly")
      .addClass("embed")
      .appendTo(fieldset);

    $(share).add(embed).click(function() {
      $(this).select();
    });


    function updateParameters() {
      for (var k in parameters) {
        if (parameters.hasOwnProperty(k)) {
          parameters[k] = $("input[name=" + k + "]", dialog).val();
        }
      }
    }

    buttons[Drupal.t("Start walkthrough")] = function () {
      updateParameters();
      if (httpproxy && !useproxy.is(":checked")) {
        state.HTTPProxyURL = null;
      }
      if (disableClicks.is(":checked")) {
        state.disableClicks = true;
      }
      var method_name = $("input[name=method]:checked", dialog).val() || "iframe";
      var playlist = [];
      $("input[type=checkbox].prerequisite", dialog).each(function () {
        if ($(this).is(":checked")) {
          playlist.push($(this).val());
        }
      });
      if (playlist) {
        playlist.push(state.walkthrough);
        state.walkthrough = playlist.shift();
        state.next = playlist;
      }
      server.startWalkthrough(parameters, methods[method_name]);
      if (!getdata.embedorigin) {
        buttons[Drupal.t("Cancel")]();
      }
    };
    buttons[Drupal.t("Cancel")] = function () {
      dialog.dialog("close");
      dialog.remove();
    };

    function regenLinks() {
      updateParameters();

      // Generate sharing link
      var link = window.location.origin + window.location.pathname + "?";
      for (var param in parameters) {
        if (parameters.hasOwnProperty(param)) {
          link += param + "=" + encodeURIComponent(parameters[param]) + "&";
        }
      }
      link = link.substr(0, link.length - 1);
      if (httpproxy) {
        link += "&useproxy=" + (useproxy.is(":checked") ? "1" : "0");
      }
      share.val(link + "&autostart=1");

      // Generate embed data
      var embedurl = walkthroughlink.data("embedjs") + "?";
      for (param in parameters) {
        if (parameters.hasOwnProperty(param)) {
          embedurl += "parameters[" + param + "]=" + encodeURIComponent(parameters[param]) + "&";
        }
      }
      embedurl = embedurl.substr(0, embedurl.length - 1);
      if (httpproxy) {
        embedurl += "&useproxy=" + (useproxy.is(":checked") ? "1" : "0");
      }

      var embedkey = walkthroughlink.data("embedjskey");
      var embeddata = "<script src=\"EMBEDURL\" type=\"application/javascript\"></script><div class=\"walkthroughbutton\" data-key=\"EMBEDKEY\"></div>"
        .replace("EMBEDURL", embedurl)
        .replace("EMBEDKEY", embedkey);

      embed.val(embeddata);
    }

    regenLinks();

    $("input", dialog)
      .blur(regenLinks)
      .keyup(regenLinks)
      .click(regenLinks)
      .change(regenLinks)
      .blur();

    if (getdata.embedorigin) {
      setTimeout(buttons[Drupal.t("Start walkthrough")], 100);
      return;
    }

    dialog.appendTo($("body"));
    dialog.dialog({
      autoOpen: true,
      modal: true,
      buttons: buttons,
      dialogClass: "walkthrough-start-dialog",
      draggable: false,
      resizable: false
    });
  }

  function flagWalkthroughAsBroken() {
    $("span.flag-walkthrough-broken a.flag-action").click();
  }

  var errormessages_alter = {
    "command-not-supported": flagWalkthroughAsBroken,
    "locator-fail": function (msg) {
      var link = $("<a/>")
        .html(Drupal.t("Mark as broken"))
        .addClass("button")
        .addClass("markbroken")
        .click(function (event) {
          event.preventDefault();
          flagWalkthroughAsBroken();
        })
        .appendTo(msg.parent());

      if (getdata.markbroken) {
        link.click();
      }
    }
  };

  function suppressErrorMessage(id) {
    $("#walkhub-error-message-" + id, methods.iframe.object.parent()).remove();
  }

  function showErrorMessage(id, error) {
    suppressErrorMessage(id);
    var msg = $("<div />")
      .attr("id", "walkhub-error-message-" + id)
      .addClass("walkhub-error-message")
      .html(error);
    $("span.ui-dialog-title", methods.iframe.object.parent()).html(msg);

    if (errormessages_alter.hasOwnProperty(id)) {
      errormessages_alter[id](msg);
    }
  }

  function flashStep(cmd, arg0, arg1) {
    var parent = methods.iframe.object && methods.iframe.object.parent && methods.iframe.object.parent() || $("body");
    var flash = $("<span />")
      .addClass("walkhub-recording-indicator")
      .text(formatStep(cmd, arg0, arg1))
      .appendTo($("span.ui-dialog-title", parent));

    setTimeout(function () {
      flash.remove();
    }, 1000);
  }

  function resizeStepsDropdown(open) {
    var $stepList = $("#stepList");
    var $steps = $stepList.find("li");

    if (!$stepList.data("collapsedHeight")) {
      $stepList.data("collapsedHeight", $stepList.outerHeight());
    }
    var height = $stepList.data("collapsedHeight");

    if ($steps.size() <= 1) {
      open = false;
    }

    if (open) {
      height = $steps.outerHeight(true) * $steps.size();
      height += parseInt($stepList.css("padding-top")) + parseInt($stepList.css("padding-bottom"));

      $stepList.data("maxHeight", "false");
      var maxHeight = $(window).height() * 0.8;
      if (height > maxHeight) {
        height = maxHeight;
        $stepList.data("maxHeight", "true");
      }
    }

    $stepList.stop();

    if (open) {
      $stepList.animate({"height": height + "px"}, 300, function () {
        if ($(this).data("maxHeight") === "true") {
          $(this).css("overflow", "scroll");
        } else {
          $(this).css("overflow", "hidden");
        }
      });
    } else {
      $stepList.animate({"height": height + "px"}, 300, function () {
        $(this).data("maxHeight", "false");
        $(this).css("overflow", "hidden");
        $(this).scrollTop(0);
      });
    }
  }

  function updateTitlebarStepList(cmd, arg0, arg1) {
    var $stepList = $("#stepList");
    var listItem = $("<li />")
      .attr("value", parseInt($stepList.find("li").size()) + 1)
      .text(formatStep(cmd, arg0, arg1))
      .prependTo($stepList);
    $("<span />")
      .addClass("icon-trash removeStep")
      .appendTo(listItem);
    if ($("#stepList:hover").size() > 0) {
      resizeStepsDropdown(true);
    }
  }

  function formatStep(cmd, arg0, arg1) {
    var text = cmd + "(";
    if (arg0) {
      text += arg0;
      if (arg1) {
        text += ", " + arg1;
      }
    }
    text += ")";

    return text;
  }

  function removeStep(n) {
    var $container = $("textarea#edit-steps");
    var stepdata = $container.val();
    var steps;
    if (stepdata) {
      steps = JSON.parse(stepdata);
    } else {
      steps = [];
    }
    steps.splice(n, 1);
    $container.val(JSON.stringify(steps));
    renderStepList($container.val());
  }

  function addStep(cmd, arg0, arg1) {
    var container = $("textarea#edit-steps");
    var stepdata = container.val();
    var steps;
    if (stepdata) {
      steps = JSON.parse(stepdata);
    } else {
      steps = [];
    }

    steps.push({
      cmd: cmd,
      arg0: arg0,
      arg1: arg1
    });

    container.val(JSON.stringify(steps));

    renderStepList(container.val());

    updateTitlebarStepList(cmd, arg0, arg1);
  }

  function renderStepList(stepsJSON) {
    var steps;
    if (stepsJSON) {
      steps = JSON.parse(stepsJSON);
    } else {
      steps = [];
    }
    var $stepsContainer = $("#steps");
    $stepsContainer.find("li").remove();

    for (var i in steps) {
      if (steps.hasOwnProperty(i)) {
        $("<li />")
          .text(formatStep(steps[i].cmd, steps[i].arg0, steps[i].arg1))
          .appendTo($stepsContainer);
      }
    }
  }

  function defaultState() {
    return {
      walkthrough: null,
      step: null,
      completed: false,
      stepIndex: 0,
      parameters: {},
      HTTPProxyURL: "",
      editLink: "",
      next: [],
      recording: false,
      recordedSteps: [],
      recordBuffer: {},
      recordStartUrl: "",
      recordStarted: false
    };
  }

  function WalkhubServer() {
    var key = Math.random().toString();
    var that = this;
    var currentURL = null;
    var state = defaultState();
    var method;

    function maybeProxy(newdata, olddata) {
      if (olddata.proxy_key) {
        newdata.proxy_key = olddata.proxy_key;
      }
      return newdata;
    }

    var handlers = {
      connect: function (data, source) {
        walkthroughOrigin = data.origin;
        currentURL = data.url;
        post(maybeProxy({
          type: "connect_ok",
          origin: window.location.origin,
          baseurl: baseurl(),
          key: key
        }, data), source);
      },
      request: function (data, source) {
        var request = function () {
          var url = data.URL;
          if (getdata.access_key) {
            url += (url.indexOf("?") === -1 ? "?" : "&") + "access_key=" + getdata.access_key;
          }
          var opts = {
            url: url,
            type: "GET",
            success: function (respdata) {
              post(maybeProxy({
                ticket: data.ticket,
                type: "success",
                data: respdata
              }, data), source);
            },
            error: function (xhr, status, err) {
              post(maybeProxy({
                ticket: data.ticket,
                type: "error",
                error: err,
                status: status
              }, data), source);
            },
            dataType: "json",
            accept: "application/json",
            headers: {
              "X-CSRF-Token": csrf_token
            }
          };

          if (data.data) {
            opts.data = JSON.stringify(data.data);
            opts.contentType = "application/json; charset=utf-8";
            opts.type = "PUT";
          }

          if (data.method) {
            opts.type = data.method;
          }

          $.ajax(opts);
        };
        if (!data.data || csrf_token) {
          request();
        } else {
          $.ajax({
            url: Drupal.settings.basePath + "services/session/token",
            dataType: "text",
            type: "GET",
            success: function (data) {
              console.log("CSRF TOKEN: " + data);
              csrf_token = data;
              request();
            }
          });
        }
      },
      getState: function (data, source) {
        post(maybeProxy({
          type: "state",
          state: state
        }, data), source);
      },
      setState: function (data, source) {
        console.log("State updated", data.state);
        state = data.state;
      },
      saveStep: function (data, source) {
        addStep(data.cmd, data.arg0, data.arg1);
      },
      enablePasswordParameter: function (data, source) {
        $("input#edit-password-parameter").attr("checked", "checked");
      },
      log: function (data, source) {
        // TODO set a variable to enable/disable logging
        console.log("REMOTE LOG", data.log);
      },
      showError: function (data, source) {
        showErrorMessage(data.id, data.error);
      },
      suppressError: function (data, source) {
        suppressErrorMessage(data.id);
      },
      finished: function (data, source) {
        embeddedPost({type: "end"});
        method.teardown();
        state = defaultState();
      },
      ping: function (data, source) {
        post({type: "pong", tag: "server"}, source, data.origin);
      }
    };

    handlers.connect.keyBypass = true;
    handlers.ping.keyBypass = true;

    function logMessage(msg, prefix) {
      if (msg.type && msg.type === "log") {
        return;
      }
      console.log(prefix + "\t" + JSON.stringify(msg));
    }

    function post(message, source, origin) {
      if (source.postMessage) {
        logMessage(message, ">>");
        source.postMessage(JSON.stringify(message), origin || walkthroughOrigin);
      } else {
        console.log("Sending message failed.");
      }
    }

    function onMessageEventHandler(event) {
      var data = JSON.parse(event.data);
      var handler = data && data.type && handlers[data.type];
      if (handler && (handler.keyBypass || (data.key && data.key === key))) {
        logMessage(data, "<<");
        handler(data, event.source);
      } else {
        console.log("Message discarded", JSON.stringify(data), event);
      }
    }

    // Add message event handlers.
    // <IE8 uses window.attachEvent() and not window.addEventListender().
    if (window.addEventListener) {
      window.addEventListener("message", onMessageEventHandler);
    } else {
      window.attachEvent("onmessage", onMessageEventHandler);
    }

    this.clickEventHandler = function (event) {
      event.preventDefault();
      state = defaultState();
      state.walkthrough = $(this).attr("data-walkthrough-uuid");
      state.HTTPProxyURL = $(this).attr("data-walkthrough-proxy-url");
      state.editLink = $(this).attr("data-walkthrough-edit-link");
      state.step = null;
      state.stepIndex = 0;
      state.parameters = {};
      state.completed = false;
      state.socialSharing = $(this).attr("data-social-sharing");
      state.next = [];
      state.allowEditing = (!$("body").hasClass("walkthrough-hide-edit"));
      currentURL = null;
      createDialogForm($(this), that, state);
    };

    this.recorderClickEventHandlerFactory = function (urlField, useProxyField) {
      return function (event) {
        event.preventDefault();

        // @TODO detect if the urlField"s value is a valid url

        state = defaultState();
        state.recording = true;
        state.recordStartUrl = urlField.val();
        addStep("open", state.recordStartUrl, null);
        if (useProxyField.is(":checked")) {
          state.HTTPProxyURL = $(this).attr("data-proxy-url");
        }
        methods.iframe.execute(baseurl() + "walkhub", true);
      };
    };

    this.startWalkthrough = function (parameters, wtmethod) {
      method = wtmethod;
      if (window.proxy) {
        window.proxy.pause();
        // TODO call window.proxy.resume() when the walkthrough finishes.
      }
      state.parameters = parameters;
      embeddedPost({type: "start"});
      method.execute(currentURL || (baseurl() + "walkhub"));
    };
  }

  var appserver = new WalkhubServer();

  Drupal.behaviors.walkhub = {
    attach: function (context) {
      $(".walkthrough-start:not(.walkhub-processed)", context)
        .addClass("walkhub-processed")
        .each(function () {
          $(this).click(appserver.clickEventHandler);
          if (getdata.autostart) {
            $(this).click();
          }
        });
      $(".walkhub-record-form:not(.walkhub-processed)", context)
        .addClass("walkhub-processed")
        .each(function () {
          $(".edit-record", $(this))
            .click(appserver.recorderClickEventHandlerFactory(
              $(".edit-url", $(this)),
              $(".edit-use-proxy", $(this))
            ));
        });
      $("#stepList, .icon-chevron-down", "#stepListContainer").live("mouseenter", function(e) {
        resizeStepsDropdown(true);
      }).live("mouseleave", function(e) {
        resizeStepsDropdown(false);
      });
      $(".removeStep", context).live("click", function() {
        var $stepList = $("#stepList");
        var n = $stepList.find("li").index($(this).parent());
        var listLength = $stepList.find("li").size();
        var prevs = $(this).parent().prevAll();
        $(this).parent().remove();
        removeStep(listLength - n);
        resizeStepsDropdown($stepList.find("li").size() !== 1);
        prevs.each(function(index) {
          $(this).attr("value", n + 1 + index);
        });
      });

      $("#stopRecording", context).live("click", function() {
        methods.iframe.object.dialog("close");
      });
    }
  };

  function baseurl() {
    return window.location.protocol + "//" + window.location.hostname + ":" + window.location.port + Drupal.settings.basePath;
  }

  function embeddedPost(msg) {
    var origin = (getdata.embedorigin && window.parent) ? getdata.embedorigin : null;
    if (origin) {
      if (!msg.origin) {
        msg.origin = decodeURIComponent(origin);
      }
      if (!msg.ticket && getdata.ticket) {
        msg.ticket = getdata.ticket;
      }
      window.parent.postMessage(JSON.stringify(msg), decodeURIComponent(origin));
    }
  }

})(jQuery);
