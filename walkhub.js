(function ($) {
  var walkthroughOrigin;

  var MAXIMUM_ZINDEX = 2147483647;
  var LINK_CHECK_TIMEOUT = 500;

  var csrf_token = null;

  var getdata = window.location.search.substr(1).split('&').reduce(function (obj, str) {
    str = str.split('=');
    obj[str.shift()] = str.join('=');
    return obj;
  }, {});

  function baseurl() {
    return window.location.protocol + '//' + window.location.hostname + Drupal.settings.basePath;
  }

  var iOS = navigator.platform === 'iPad' || navigator.platform === 'iPhone' || navigator.platform === 'iPod';

  var methods = {
    iframe: {
      name: 'iFrame',
      linkcheck: false,
      execute: function (url) {
        var iframe = $('<iframe />')
          .attr('src', url)
          .attr('frameborder', 0)
          .attr('scrolling', 'auto')
          .attr('allowtransparency', 'true');

        iframe
          .appendTo($('body'))
          .dialog({
            modal: true,
            autoOpen: true
          });

        function resize() {
          iframe.dialog('option', 'width', $(window).width() - 20);
          iframe.dialog('option', 'height', $(window).height() - 20);
          iframe.css('width', '100%');
        }

        resize();

        window.addEventListener('resize', resize);

        iframe
          .parent()
            .css('z-index', MAXIMUM_ZINDEX);

        return iframe.get(0).contentWindow;
      },
      valid: true
    },
    popup: {
      name: 'Popup',
      linkcheck: true,
      execute: function (url) {
        return window.open(url);
      },
      valid: !iOS
    }
  };

  function getDefaultParameters(walkthroughlink) {
    var parameters = {};
    data = walkthroughlink.data();

    for (var k in data) {
      if (k.indexOf('walkthroughParameter') == 0) {
        var parameter = k.substr('walkthroughParameter'.length).toLowerCase();
        var default_value = data[k];
        parameters[parameter] = getdata[parameter] || default_value;
      }
    }

    return parameters;
  }

  function createDialogForm(walkthroughlink, server) {
    var parameters = getDefaultParameters(walkthroughlink);
    var dialog = $('<div />')
      .attr('id', 'walkthrough-dialog-' + Math.random().toString())
      .attr('title', Drupal.t('Start Walkthrough'))
      .addClass('walkthrough-dialog')
      .hide()
      .append($('<form><fieldset></fieldset></form>'));
    var fieldset = dialog.find('fieldset');
    $('<p />')
      .html(Drupal.t('The following parameters are available in this walkthrough:'))
      .appendTo(fieldset);

    for (var parameter in parameters) {
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

    function updateParameters() {
      for (var k in parameters) {
        parameters[k] = $('input[name=' + k + ']', dialog).val();
      }
    }

    var buttons = {};
    buttons[Drupal.t('Start walkthrough')] = function () {
      updateParameters();
      var method_name = $('input[name=method]:checked', dialog).val();
      server.startWalkthrough(parameters, methods[method_name]);
      buttons[Drupal.t('Cancel')]();
    };
    buttons[Drupal.t('Cancel')] = function () {
      dialog.dialog('close');
      dialog.remove();
    };

    $('<label />')
      .attr('for', 'sharelink')
      .html(Drupal.t('Share with these parameters: '))
      .appendTo(dialog.find('form'));

    var share = $('<textarea />')
      .attr('name', 'sharelink')
      .attr('readonly', 'readonly')
      .addClass('share')
      .appendTo(dialog.find('form'));

    function regenLinks() {
      updateParameters();
      var link = window.location.origin + window.location.pathname + '?';
      for (var parameter in parameters) {
        link += parameter + '=' + encodeURIComponent(parameters[parameter]) + '&';
      }
      link = link.substr(0, link.length - 1);
      share.val(link + '&autostart=1');
    }

    regenLinks();

    $('input', dialog)
      .blur(regenLinks)
      .keyup(regenLinks);

    dialog.appendTo($('body'));
    dialog.dialog({
      autoOpen: true,
      modal: true,
      buttons: buttons,
      dialogClass: 'walkthrough-start-dialog'
    });
  }

  function createInProgressDialog(frame, cancel) {
    var dialog = $('<div />')
      .attr('id', 'walkthrough-in-progress-dialog-' + Math.random().toString())
      .attr('title', Drupal.t('Walkthrough is in progress'))
      .hide();

    dialog.append($('<p />').html(Drupal.t('Please do not close this window. This message will disappear when you finish your walkthrough.')));

    return dialog.dialog({
      autoOpen: true,
      modal: true,
      closeOnEscape: false,
      dialogClass: 'walkthrough-in-progress',
      buttons: {
        'Cancel walkthrough': function () {
          if (cancel) {
            cancel();
          }
          frame.close();
        }
      }
    });
  }

  function WalkhubServer() {
    var key = Math.random().toString();

    var self = this;

    var currentURL = null;

    var state = {
      walkthrough: null,
      step: null,
      completed: false,
      stepIndex: 0,
      parameters: {}
    };

    var finished = false;

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
        window.console && console.log && console.log('REMOTE LOG', data.log);
      },
      finished: function (data, source) {
        finished = true;
      },
      ping: function (data, source) {
        post({type: 'pong', tag: 'server'}, source, data.origin);
      }
    };

    handlers.connect.keyBypass = true;
    handlers.ping.keyBypass = true;

    function post(message, source, origin) {
      if (source.postMessage) {
        source.postMessage(JSON.stringify(message), origin || walkthroughOrigin);
      } else {
        window.console && console.log && console.log('Sending message failed.');
      }
    }

    window.addEventListener('message', function (event) {
      var data = JSON.parse(event.data);
      var handler = data && data.type && handlers[data.type];
      if (handler && (handler.keyBypass || (data.key && data.key == key))) {
        console.log(event);
        handler(data, event.source);
      } else {
        console.log('Message discarded', event);
      }
    });

    this.clickEventHandler = function (event) {
      event.preventDefault();
      state.walkthrough = $(this).attr('data-walkthrough-uuid');
      state.step = null;
      state.stepIndex = 0;
      state.parameters = {};
      state.completed = false;
      finished = false;
      currentURL = null;
      createDialogForm($(this), self);
    };

    this.startWalkthrough = function (parameters, method) {
      if (window.proxy) {
        window.proxy.pause();
        // TODO call window.proxy.resume() when the walkthrough finishes.
      }
      state.parameters = parameters;
      var wtwindow = method.execute(currentURL || (baseurl() + 'walkhub#' + window.location.origin));
      if (!wtwindow) {
        return;
      }

      if (method.linkcheck) {
        var dialog = createInProgressDialog(wtwindow, function () {
          finished = true;
        });

        function checkLink() {
          if (finished || wtwindow.closed) {
            dialog.dialog('close');
          }
          if (wtwindow.closed) {
            if (!finished) {
              var cancel = function () {};
              Walkhub.showExitDialog(Drupal.t('Walkthrough is closed while it was in progress.'), {
                'Reopen': function () {
                  self.startWalkthrough(parameters, method);
                },
                'Cancel': cancel
              }, cancel);
            } else {
              // Tear down the server
            }
          } else {
            setTimeout(checkLink, LINK_CHECK_TIMEOUT);
          }
        }

        checkLink();
      }
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
})(jQuery);
