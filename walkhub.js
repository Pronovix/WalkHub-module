(function ($) {
  var walkthroughOrigin;

  var getdata = window.location.search.substr(1).split('&').reduce(function (obj, str) {
    str = str.split('=');
    obj[str.shift()] = str.join('=');
    return obj;
  }, {});

  function baseurl() {
    return window.location.protocol + '//' + window.location.hostname + Drupal.settings.basePath;
  }

  function getDefaultTokens(walkthroughlink) {
    var tokens = {};
    data = walkthroughlink.data();

    for (var k in data) {
      if (k.indexOf('walkthroughToken') == 0) {
        var token = k.substr('walkthroughToken'.length).toLowerCase();
        var default_value = data[k];
        tokens[token] = getdata[token] || default_value;
      }
    }

    return tokens;
  }

  function createDialogForm(walkthroughlink, server) {
    var tokens = getDefaultTokens(walkthroughlink);
    var dialog = $('<div />')
      .attr('id', 'walkthrough-dialog-' + Math.random().toString())
      .attr('title', Drupal.t('Walkthrough parameters'))
      .hide()
      .append($('<form><fieldset></fieldset></form>'));
    var fieldset = dialog.find('fieldset');
    for (var token in tokens) {
      $('<label/>')
        .attr('for', token)
        .html(token)
        .appendTo(fieldset);
      $('<input />')
        .attr('type', 'text')
        .attr('name', token)
        .attr('value', tokens[token])
        .attr('id', token)
        .addClass('text')
        .addClass('ui-widget-content')
        .addClass('ui-corner-all')
        .appendTo(fieldset);
    }

    function updateTokens() {
      for (var k in tokens) {
        tokens[k] = $('input[name=' + k + ']', dialog).val();
      }
    }

    var buttons = {};
    buttons[Drupal.t('Start walkthrough')] = function () {
      updateTokens();
      server.startWalkthrough(tokens);
      buttons[Drupal.t('Cancel')]();
    };
    buttons[Drupal.t('Cancel')] = function () {
      dialog.dialog('close');
      dialog.remove();
    };

    var simplelink = $('<a />').html(Drupal.t('Permalink'));
    var autolink = $('<a />').html(Drupal.t('Automatic start link'));

    function regenLinks() {
      updateTokens();
      var link = window.location.href + '?';
      for (var token in tokens) {
        link += token + '=' + encodeURIComponent(tokens[token]) + '&';
      }
      link = link.substr(0, link.length - 1);
      simplelink.attr('href', link);
      autolink.attr('href', link + '&autostart=1');
    }

    dialog.append($('<p />').append(simplelink));
    dialog.append($('<p />').append(autolink));

    regenLinks();

    $('input', dialog)
      .blur(regenLinks)
      .keyup(regenLinks);

    dialog.appendTo($('body'));
    dialog.dialog({
      autoOpen: true,
      modal: true,
      buttons: buttons
    });
  }

  function WalkhubServer() {
    var key = Math.random().toString();

    var self = this;

    var state = {
      walkthrough: null,
      step: null,
      completed: false,
      stepIndex: 0,
      tokens: {}
    };

    function maybeProxy(newdata, olddata) {
      if (olddata.proxy_key) {
        newdata.proxy_key = olddata.proxy_key;
      }
      return newdata;
    }

    var handlers = {
      connect: function (data, source) {
        walkthroughOrigin = data.origin;
        post(maybeProxy({
          type: 'connect_ok',
          origin: window.location.origin,
          baseurl: baseurl(),
          key: key
        }, data), source);
      },
      request: function (data, source) {
        var opts = {
          url: data.URL,
          method: 'GET',
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
          accept: 'application/json'
        };

        if (data.data) {
          opts.data = data;
          opts.method = 'POST';
        }

        $.ajax(opts);
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
      }
    };

    handlers.connect.keyBypass = true;

    function post(message, source) {
      if (source.postMessage) {
        source.postMessage(JSON.stringify(message), walkthroughOrigin);
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
      if ($(this).attr('data-walkthrough-has-tokens')) {
        createDialogForm($(this), self);
      } else {
        self.startWalkthrough({});
      }
    };

    this.startWalkthrough = function (tokens) {
      state.tokens = tokens;
      window.open(baseurl() + 'walkhub#' + window.location.origin);
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
