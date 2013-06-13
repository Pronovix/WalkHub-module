(function ($) {
  var walkthroughOrigin;

  function baseurl() {
    return window.location.protocol + '//' + window.location.hostname + Drupal.settings.basePath;
  }

  function WalkhubServer() {
    var key = Math.random().toString();

    var state = {
      walkthrough: null,
      step: null,
      completed: false,
      stepIndex: 0
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
        });
    }
  };
})(jQuery);
