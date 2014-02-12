// Add window.location.origin for browsers which doesn't support it.
if (!window.location.origin) {
  window.location.origin = window.location.protocol + "//" + window.location.hostname + (window.location.port ? ':' + window.location.port : '');
}

// Add console log function if not availible.
if (!window.console || !window.console.log) {
  window.console = {
    log: function log() {
      "use strict";
    }
  };
}
