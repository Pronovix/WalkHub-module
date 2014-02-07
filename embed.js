(function ($) {
  window.WalkhubEmbedData = window.WalkhubEmbedData || {};

  if (window.WalkhubEmbedJS) {
    return;
  }

  window.WalkhubEmbedJS = true;

  function fixIFrameSize(iframe) {
    iframe
      .css('width', '215px')
      .css('height', '70px');
  }

  $(function () {
    $('div.walkthroughbutton:not(.processed)')
      .addClass('processed')
      .each(function () {
        var key =  $(this).data('key');
        if (!key) {
          return;
        }

        var data = window.WalkhubEmbedData[key];
        if (!data) {
          return;
        }

        var ticket = Math.random().toString();

        var iframe = $('<iframe />')
          .attr('src', data['buttonuri'] +
            '&embedorigin=' + encodeURIComponent(window.location.origin) +
            '&ticket=' + ticket)
          .attr('frameborder', 0)
          .attr('scrolling', 'auto')
          .attr('allowtransparency', 'true');

        fixIFrameSize(iframe);

        $(this)
          .append(iframe)
          .data('ticket', ticket);
      });
  });

  window.addEventListener('message', function (event) {
    var msg = JSON.parse(event.data);
    var iframe = $('div.walkthroughbutton')
      .filter(function () { return $(this).data('ticket') == msg.ticket; })
      .find('iframe');
    switch (msg.type) {
      case 'start':
        iframe
          .css('position', 'fixed')
          .css('z-index', 2147483647)
          .css('left', '0px')
          .css('top', '0px');
        $(window).bind('resize.whembed' + msg.ticket, function () {
          iframe
            .css('width', $(window).width() + 'px')
            .css('height', $(window).height() + 'px');
        });
        $(window).resize();
        break;
      case 'end':
        $(window).unbind('resize.whembed' + msg.ticket);
        iframe.attr('style', '');
        fixIFrameSize(iframe);
        break;
    }
  });
})(jqWalkhub);
