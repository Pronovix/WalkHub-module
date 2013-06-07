(function ($) {
  var walkthroughOrigin;

  function baseurl() {
    return window.location.protocol + '//' + window.location.hostname + Drupal.settings.basePath;
  }

  var state = {
    walkthrough: null,
    step: null,
    completed: false,
    stepIndex: 0
  };

  var handlers = {
    connect: function (data, source) {
      walkthroughOrigin = data.origin;
      post({
        type: 'connect_ok',
        origin: window.location.origin,
        baseurl: baseurl()
      }, source);
    },
    request: function (data, source) {
      var opts = {
        url: data.URL,
        method: 'GET',
        success: function (respdata) {
          post({
            ticket: data.ticket,
            type: 'success',
            data: respdata
          }, source);
        },
        error: function (xhr, status, err) {
          post({
            ticket: data.ticket,
            type: 'error',
            error: err,
            status: status
          }, source);
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
      post({
        type: 'state',
        state: state
      }, source);
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

  function post(message, source) {
    if (source.postMessage) {
      source.postMessage(JSON.stringify(message), walkthroughOrigin);
    } else {
      window.console && console.log && console.log('Sending message failed.');
    }
  }

  window.addEventListener('message', function (event) {
    var data = JSON.parse(event.data);
    if (data && data.type && handlers[data.type]) {
      console.log(event);
      handlers[data.type](data, event.source);
    }
  });

  Drupal.behaviors.walkhub = {
    attach: function (context) {
      $('.walkthrough-start:not(.walkhub-processed)', context)
        .addClass('walkhub-processed')
        .click(function (event) {
           event.preventDefault();
           state.walkthrough = $(this).attr('data-walkthrough-uuid');
           state.step = null;
           window.open(baseurl() + 'walkhub#' + window.location.origin);
         });
    }
  };
})(jQuery);
