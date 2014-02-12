(function ($) {
  "use strict";

  var getdata = window.location.search.substr(1).split('&').reduce(function (obj, str) {
      str = str.split('=');
      obj[str.shift()] = str.join('=');
      return obj;
    }, {}),
    MAXIMUM_ZINDEX = 2147483647,
    walkthroughOrigin,
    csrf_token = null,

  // @TODO convert these into proper objects. Remove the singleton state of methods.*.object.
    methods = {
      iframe: {
        name: 'iFrame',
        linkcheck: false,
        execute: function (url) {
          var widget,
            iframe = $('<iframe />')
              .attr('src', url)
              .attr('frameborder', 0)
              .attr('scrolling', 'auto')
              .attr('allowtransparency', 'true');

          methods.iframe.object = iframe;

          iframe
            .appendTo($('body'))
            .dialog({
              modal: true,
              autoOpen: true,
              draggable: false,
              resizable: false
            });

          widget = iframe.dialog('widget');

          function resize() {
            var width = $(window).width() - 20,
              height = $(window).height() - 20;

            // If full window is required.
            if ($('body').hasClass('walkthrough-full-window')) {
              width = $(window).width();
              height = $(window).height();
              // Hide dialog title.
              $('.ui-dialog-titlebar', widget).hide();
              // Make the dialog display in full window.
              widget.css('top', '0px');
              widget.css('bottom', '0px');
              widget.css('left', '0px');
              widget.css('right', '0px');
            }

            iframe.dialog('option', 'width', width);
            iframe.dialog('option', 'height', height);
            iframe.dialog('option', 'position', 'center');

            widget.css('padding', '0px');
            widget.css('margin', '0px');
            widget.css('border', 'none');

            iframe.css('width', width);
            iframe.css('height', height);
            iframe.css('position', 'center');
          }

          resize();

          window.addEventListener('resize', resize);

          iframe
            .parent()
            .css('z-index', MAXIMUM_ZINDEX);

          if (getdata.embedorigin) {
            setTimeout(function () {
              $('.ui-dialog-titlebar-close').click(function () {
                embeddedPost({type: 'end'});
              });
            }, 500);
          }

          return iframe.get(0).contentWindow;
        },
        teardown: function () {
          if (methods.iframe.object) {
            methods.iframe.object.dialog('close');
            methods.iframe.object.remove();
          }
        },
        valid: true
      }
    };


  function jqCompat(version) {
    var jqversionparts = $.fn.jquery.split('.'),
      versionparts = version.split('.'),
      p;

    for (p in versionparts) {
      if (!versionparts.hasOwnProperty(p)) {
        continue;
      }
      if (versionparts[p] > (jqversionparts[p] || 0)) {
        return false;
      }
      if (versionparts[p] < (jqversionparts[p] || 0)) {
        return true;
      }
    }

    return true;
  }

  function getDefaultParameters(walkthroughlink) {
    var parameters = {},
      data = walkthroughlink.data(),
      wtParamPrefix = jqCompat('1.6') ? 'walkthroughParameter' : 'walkthrough-parameter-',
      k,
      parameter,
      default_value;

    for (k in data) {
      if (data.hasOwnProperty(k) && k.indexOf(wtParamPrefix) === 0) {
        parameter = k.substr(wtParamPrefix.length).toLowerCase();
        default_value = data[k];
        parameters[parameter] = getdata[parameter] || default_value;
      }
    }

    return parameters;
  }


  function createDialogForm(walkthroughlink, server, state) {
    var parameters = getDefaultParameters(walkthroughlink),
      dialog = $('<div />')
        .attr('id', 'walkthrough-dialog-' + Math.random().toString())
        .attr('title', Drupal.t('Start Walkthrough'))
        .addClass('walkthrough-dialog')
        .hide()
        .append($('<form><fieldset></fieldset></form>')),
      fieldset = dialog.find('fieldset'),
      basepath = baseurl(),
      buttons = {},
      key,
      href,
      parameter,
      httpproxy,
      share,
      useproxy,
      k,
      embed;


    // Drupal.settings.walkhub.prerequisites stores walkthrough prerequisites.
    if (Drupal.settings.walkhub !== undefined && Drupal.settings.walkhub.prerequisites !== undefined) {
      $('<p />')
          .html(Drupal.t('Before this Walkthrough can run you need to:'))
          .appendTo(fieldset);

      for (key in Drupal.settings.walkhub.prerequisites) {
        if (Drupal.settings.walkhub.prerequisites.hasOwnProperty(key)) {
          href = basepath + "node/" + Drupal.settings.walkhub.prerequisites[key].nid;
          $('<a href="' + href + '" target="_blank" class="button">' + Drupal.settings.walkhub.prerequisites[key].title + '</a>').appendTo(fieldset);
        }
      }
    }

    $('<p />')
      .html(Drupal.t('The following parameters are available in this walkthrough:'))
      .appendTo(fieldset);

    for (parameter in parameters) {
      if (!parameters.hasOwnProperty(parameter)) {
        continue;
      }
      $('<label/>')
        .attr('for', parameter)
        .html(parameter)
        .appendTo(fieldset);
      $('<input />')
        .attr('type', 'text')
        .attr('name', parameter)
        .attr('value', parameters[parameter])
        .attr('id', parameter)
        .addClass('text')
        .addClass('ui-widget-content')
        .addClass('ui-corner-all')
        .appendTo(fieldset);
    }

    httpproxy = !!walkthroughlink.attr('data-walkthrough-proxy-url');

    $('<label />')
      .attr('for', 'sharelink')
      .html(Drupal.t('Share with these parameters: '))
      .appendTo(dialog.find('form'));

    share = $('<textarea />')
      .attr('name', 'sharelink')
      .attr('readonly', 'readonly')
      .addClass('share')
      .appendTo(dialog.find('form'));

    $('<label />')
      .attr('for', 'embedlink')
      .html(Drupal.t('Embed with these parameters: '))
      .appendTo(dialog.find('form'));

    embed = $('<textarea />')
      .attr('name', 'embedlink')
      .attr('readonly', 'readonly')
      .addClass('embed')
      .appendTo(dialog.find('form'));

    useproxy = null;
    if (httpproxy) {
      $('<label />')
        .attr('for', 'useproxy')
        .html(Drupal.t('Use proxy'))
        .appendTo(dialog.find('form'));
      useproxy = $('<input />')
        .attr('type', 'checkbox')
        .attr('name', 'useproxy')
        .attr('id', 'useproxy')
        .appendTo(dialog.find('form'));

      if (getdata['useproxy'] !== '0') {
        useproxy.attr('checked', 'checked');
      }
    }

    function updateParameters() {
      for (k in parameters) {
        if (!parameters.hasOwnProperty(k)) {
          continue;
        }
        parameters[k] = $('input[name=' + k + ']', dialog).val();
      }
    }

    buttons[Drupal.t('Start walkthrough')] = function () {
      updateParameters();
      if (httpproxy && !useproxy.is(':checked')) {
        state.HTTPProxyURL = null;
      }
      var method_name = $('input[name=method]:checked', dialog).val() || 'iframe';
      server.startWalkthrough(parameters, methods[method_name]);
      if (!getdata['embedorigin']) {
        buttons[Drupal.t('Cancel')]();
      }
    };
    buttons[Drupal.t('Cancel')] = function () {
      dialog.dialog('close');
      dialog.remove();
    };

    function regenLinks() {
      updateParameters();
      var parameter,
        link,
        embedurl,
        embedkey,
        embeddata;

      // Generate sharing link
      link = window.location.origin + window.location.pathname + '?';
      for (parameter in parameters) {
        if (!parameters.hasOwnProperty(parameter)) {
          continue;
        }
        link += parameter + '=' + encodeURIComponent(parameters[parameter]) + '&';
      }
      link = link.substr(0, link.length - 1);
      if (httpproxy) {
        link += '&useproxy=' + (useproxy.is(':checked') ? '1' : '0');
      }
      share.val(link + '&autostart=1');

      // Generate embed data
      embedurl = walkthroughlink.data('embedjs') + '?';
      for (parameter in parameters) {
        if (!parameters.hasOwnProperty(parameter)) {
          continue;
        }

        embedurl += 'parameters[' + parameter + ']=' + encodeURIComponent(parameters[parameter]) + '&';
      }
      embedurl = embedurl.substr(0, embedurl.length - 1);
      if (httpproxy) {
        embedurl += '&useproxy=' + (useproxy.is(':checked') ? '1' : '0');
      }
      embedkey = walkthroughlink.data('embedjskey');
      embeddata = "<script src=\"EMBEDURL\" type=\"application/javascript\"></script><div class=\"walkthroughbutton\" data-key=\"EMBEDKEY\"></div>"
        .replace('EMBEDURL', embedurl)
        .replace('EMBEDKEY', embedkey);

      embed.val(embeddata);
    }

    regenLinks();

    $('input', dialog)
      .blur(regenLinks)
      .keyup(regenLinks)
      .click(regenLinks)
      .change(regenLinks)
      .blur();

    if (getdata['embedorigin']) {
      setTimeout(buttons[Drupal.t('Start walkthrough')], 100);
      return;
    }

    dialog.appendTo($('body'));
    dialog.dialog({
      autoOpen: true,
      modal: true,
      buttons: buttons,
      dialogClass: 'walkthrough-start-dialog',
      draggable: false,
      resizable: false
    });
  }

  function flagWalkthroughAsBroken() {
    $('span.flag-walkthrough-broken a.flag-action').click();
  }

  var errormessages_alter = {
    'command-not-supported': flagWalkthroughAsBroken,
    'locator-fail': function (msg) {
      var link = $('<a/>')
        .html(Drupal.t('Mark as broken'))
        .addClass('button')
        .addClass('markbroken')
        .click(function (event) {
          event.preventDefault();
          flagWalkthroughAsBroken();
        })
        .appendTo(msg);

      if (getdata['markbroken']) {
        link.click();
      }
    }
  };

  function suppressErrorMessage(id) {
    $('#walkhub-error-message-' + id, methods.iframe.object.parent()).remove();
  }

  function showErrorMessage(id, error) {
    suppressErrorMessage(id);
    var msg = $('<div />')
      .attr('id', 'walkhub-error-message-' + id)
      .addClass('walkhub-error-message')
      .html(error)
      .appendTo($('span.ui-dialog-title', methods.iframe.object.parent()));

    if (errormessages_alter.hasOwnProperty(id)) {
      errormessages_alter[id](msg);
    }
  }

  function WalkhubServer() {
    var key = Math.random().toString(),
      self = this,
      currentURL = null,
      state = {
        walkthrough: null,
        step: null,
        completed: false,
        stepIndex: 0,
        parameters: {},
        HTTPProxyURL: ''
      },
      method,
      finished = false;

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
          type: 'connect_ok',
          origin: window.location.origin,
          baseurl: baseurl(),
          key: key
        }, data), source);
      },
      request: function (data, source) {
        var request = function () {
          var opts = {
            url: data.URL,
            type: 'GET',
            success: function (respdata) {
              post(maybeProxy({
                ticket: data.ticket,
                type: 'success',
                data: respdata
              }, data), source);
            },
            error: function (xhr, status, err) {
              post(maybeProxy({
                ticket: data.ticket,
                type: 'error',
                error: err,
                status: status
              }, data), source);
            },
            dataType: 'json',
            accept: 'application/json',
            headers: {
              'X-CSRF-Token': csrf_token
            }
          };

          if (data.data) {
            opts.data = JSON.stringify(data.data);
            opts.contentType = 'application/json; charset=utf-8';
            opts.type = 'PUT';
          }

          $.ajax(opts);
        };
        if (!data.data || csrf_token) {
          request();
        } else {
          $.ajax({
            url: Drupal.settings.basePath + 'services/session/token',
            dataType: 'text',
            type: 'GET',
            success: function (data) {
              console.log('CSRF TOKEN: ' + data);
              csrf_token = data;
              request();
            }
          });
        }
      },
      getState: function (data, source) {
        post(maybeProxy({
          type: 'state',
          state: state
        }, data), source);
      },
      setState: function (data, source) {
        console.log("State updated", data.state);
        state = data.state;
      },
      log: function (data, source) {
        // TODO set a variable to enable/disable logging
        console.log('REMOTE LOG', data.log);
      },
      showError: function (data, source) {
        showErrorMessage(data.id, data.error);
      },
      suppressError: function (data, source) {
        suppressErrorMessage(data.id);
      },
      finished: function (data, source) {
        finished = true;
        embeddedPost({type: 'end'});
        method.teardown();
      },
      ping: function (data, source) {
        post({type: 'pong', tag: 'server'}, source, data.origin);
      }
    };

    handlers.connect.keyBypass = true;
    handlers.ping.keyBypass = true;

    function logMessage(msg, prefix) {
      if (msg.type && msg.type === 'log') {
        return;
      }
      console.log(prefix + "\t" + JSON.stringify(msg));
    }

    function post(message, source, origin) {
      if (source.postMessage) {
        logMessage(message, ">>");
        source.postMessage(JSON.stringify(message), origin || walkthroughOrigin);
      } else {
        console.log('Sending message failed.');
      }
    }

    window.addEventListener('message', function (event) {
      var data = JSON.parse(event.data),
        handler = data && data.type && handlers[data.type];
      if (handler && (handler.keyBypass || (data.key && data.key === key))) {
        logMessage(data, "<<");
        handler(data, event.source);
      } else {
        console.log('Message discarded', event);
      }
    });

    this.clickEventHandler = function (event) {
      event.preventDefault();
      state.walkthrough = $(this).attr('data-walkthrough-uuid');
      state.HTTPProxyURL = $(this).attr('data-walkthrough-proxy-url');
      state.step = null;
      state.stepIndex = 0;
      state.parameters = {};
      state.completed = false;
      finished = false;
      currentURL = null;
      createDialogForm($(this), self, state);
    };

    this.startWalkthrough = function (parameters, wtmethod) {
      method = wtmethod;
      if (window.proxy) {
        window.proxy.pause();
        // TODO call window.proxy.resume() when the walkthrough finishes.
      }
      state.parameters = parameters;
      embeddedPost({type: 'start'});
      method.execute(currentURL || (baseurl() + 'walkhub'));
    };
  }

  Drupal.behaviors.walkhub = {
    attach: function (context) {
      $('.walkthrough-start:not(.walkhub-processed)', context)
        .addClass('walkhub-processed')
        .each(function () {
          var appserver = new WalkhubServer();
          $(this).click(appserver.clickEventHandler);
          if (getdata.autostart) {
            $(this).click();
          }
        });
    }
  };

  function baseurl() {
    return window.location.protocol + '//' + window.location.hostname + ':' + window.location.port + Drupal.settings.basePath;
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
