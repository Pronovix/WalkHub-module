<?php
/**
 * @file
 * walkhub.services.inc
 */

/**
 * Implements hook_default_services_endpoint().
 */
function walkhub_default_services_endpoint() {
  $export = array();

  $endpoint = new stdClass();
  $endpoint->disabled = FALSE; /* Edit this to true to make a default endpoint disabled initially */
  $endpoint->api_version = 3;
  $endpoint->name = 'apiv1';
  $endpoint->server = 'rest_server';
  $endpoint->path = 'api/v1';
  $endpoint->authentication = array(
    'services' => 'services',
  );
  $endpoint->server_settings = array(
    'formatters' => array(
      'json' => TRUE,
      'bencode' => FALSE,
      'jsonp' => FALSE,
      'php' => FALSE,
      'rss' => FALSE,
      'xml' => FALSE,
    ),
    'parsers' => array(
      'application/json' => TRUE,
      'application/x-www-form-urlencoded' => TRUE,
      'multipart/form-data' => TRUE,
      'application/vnd.php.serialized' => FALSE,
      'application/xml' => FALSE,
      'text/xml' => FALSE,
    ),
  );
  $endpoint->resources = array(
    'comment' => array(
      'operations' => array(
        'create' => array(
          'enabled' => '1',
        ),
        'retrieve' => array(
          'enabled' => '1',
        ),
        'update' => array(
          'enabled' => '1',
        ),
        'delete' => array(
          'enabled' => '1',
        ),
        'index' => array(
          'enabled' => '1',
        ),
      ),
      'actions' => array(
        'countAll' => array(
          'enabled' => '1',
        ),
        'countNew' => array(
          'enabled' => '1',
        ),
      ),
    ),
    'node' => array(
      'operations' => array(
        'retrieve' => array(
          'enabled' => '1',
        ),
        'create' => array(
          'enabled' => '1',
        ),
        'update' => array(
          'enabled' => '1',
        ),
        'delete' => array(
          'enabled' => '1',
        ),
        'index' => array(
          'enabled' => '1',
        ),
      ),
      'relationships' => array(
        'files' => array(
          'enabled' => '1',
        ),
        'comments' => array(
          'enabled' => '1',
        ),
      ),
      'targeted_actions' => array(
        'attach_file' => array(
          'enabled' => '1',
        ),
      ),
    ),
    'system' => array(
      'actions' => array(
        'connect' => array(
          'enabled' => '1',
        ),
      ),
    ),
    'user' => array(
      'actions' => array(
        'login' => array(
          'enabled' => '1',
        ),
        'logout' => array(
          'enabled' => '1',
          'settings' => array(
            'services' => array(
              'resource_api_version' => '1.0',
            ),
          ),
        ),
      ),
    ),
    'views' => array(
      'operations' => array(
        'retrieve' => array(
          'enabled' => '1',
        ),
        'index' => array(
          'enabled' => '1',
        ),
      ),
    ),
  );
  $endpoint->debug = 0;
  $export['apiv1'] = $endpoint;

  $endpoint = new stdClass();
  $endpoint->disabled = FALSE; /* Edit this to true to make a default endpoint disabled initially */
  $endpoint->api_version = 3;
  $endpoint->name = 'apiv2';
  $endpoint->server = 'rest_server';
  $endpoint->path = 'api/v2';
  $endpoint->authentication = array(
    'services' => 'services',
  );
  $endpoint->server_settings = array(
    'formatters' => array(
      'json' => TRUE,
      'jsonp' => TRUE,
      'bencode' => FALSE,
      'php' => FALSE,
      'xml' => FALSE,
    ),
    'parsers' => array(
      'application/json' => TRUE,
      'application/x-www-form-urlencoded' => TRUE,
      'multipart/form-data' => TRUE,
      'application/vnd.php.serialized' => FALSE,
      'application/xml' => FALSE,
      'text/xml' => FALSE,
    ),
  );
  $endpoint->resources = array(
    'system' => array(
      'actions' => array(
        'connect' => array(
          'enabled' => '1',
        ),
      ),
    ),
    'user' => array(
      'actions' => array(
        'login' => array(
          'enabled' => '1',
          'settings' => array(
            'services' => array(
              'resource_api_version' => '1.0',
            ),
          ),
        ),
        'logout' => array(
          'enabled' => '1',
          'settings' => array(
            'services' => array(
              'resource_api_version' => '1.0',
            ),
          ),
        ),
      ),
    ),
    'walkhub-walkthrough' => array(
      'operations' => array(
        'retrieve' => array(
          'enabled' => '1',
        ),
      ),
    ),
    'walkhub-step' => array(
      'operations' => array(
        'retrieve' => array(
          'enabled' => '1',
        ),
        'update' => array(
          'enabled' => '1',
        ),
      ),
    ),
    'walkhub-list' => array(
      'operations' => array(
        'retrieve' => array(
          'enabled' => '1',
        ),
        'index' => array(
          'enabled' => '1',
        ),
      ),
    ),
    'walkhub-listing' => array(
      'operations' => array(
        'index' => array(
          'enabled' => '1',
        ),
      ),
    ),
    'walkthrough-phpunit' => array(
      'operations' => array(
        'retrieve' => array(
          'enabled' => '1',
        ),
      ),
    ),
    'walkhub-branding' => array(
      'operations' => array(
        'retrieve' => array(
          'enabled' => '1',
        ),
      ),
    ),
    'walkhub-collection' => array(
      'operations' => array(
        'retrieve' => array(
          'enabled' => '1',
        ),
        'index' => array(
          'enabled' => '1',
        ),
      ),
    ),
    'walkhub-walkthrough-screening-queue' => array(
      'operations' => array(
        'index' => array(
          'enabled' => '1',
        ),
      ),
    ),
    'walkhub-user' => array(
      'actions' => array(
        'registration' => array(
          'enabled' => '1',
        ),
      ),
    ),
    'walkthrough-set' => array(
      'operations' => array(
        'retrieve' => array(
          'enabled' => '1',
        ),
      ),
    ),
    'walkthrough-set-phpunit' => array(
      'operations' => array(
        'retrieve' => array(
          'enabled' => '1',
        ),
      ),
    ),
  );
  $endpoint->debug = 0;
  $export['apiv2'] = $endpoint;

  $endpoint = new stdClass();
  $endpoint->disabled = FALSE; /* Edit this to true to make a default endpoint disabled initially */
  $endpoint->api_version = 3;
  $endpoint->name = 'apiv2_oauth';
  $endpoint->server = 'rest_server';
  $endpoint->path = 'api/v2/oauth';
  $endpoint->authentication = array(
    'services' => 'services',
    'services_oauth' => array(
      'oauth_context' => 'walkthrough_selenium_executor',
      'credentials' => 'consumer',
    ),
  );
  $endpoint->server_settings = array(
    'formatters' => array(
      'json' => TRUE,
      'jsonp' => TRUE,
      'bencode' => FALSE,
      'php' => FALSE,
      'xml' => FALSE,
    ),
    'parsers' => array(
      'application/json' => TRUE,
      'application/x-www-form-urlencoded' => TRUE,
      'multipart/form-data' => TRUE,
      'application/vnd.php.serialized' => FALSE,
      'application/xml' => FALSE,
      'text/xml' => FALSE,
    ),
  );
  $endpoint->resources = array(
    'system' => array(
      'actions' => array(
        'connect' => array(
          'enabled' => '1',
          'settings' => array(
            'services_oauth' => array(
              'credentials' => '',
              'authorization' => '',
            ),
          ),
        ),
      ),
    ),
    'user' => array(
      'actions' => array(
        'login' => array(
          'enabled' => '1',
          'settings' => array(
            'services' => array(
              'resource_api_version' => '1.0',
            ),
            'services_oauth' => array(
              'credentials' => '',
              'authorization' => '',
            ),
          ),
        ),
        'logout' => array(
          'enabled' => '1',
          'settings' => array(
            'services' => array(
              'resource_api_version' => '1.0',
            ),
            'services_oauth' => array(
              'credentials' => '',
              'authorization' => '',
            ),
          ),
        ),
        'token' => array(
          'enabled' => '1',
          'settings' => array(
            'services_oauth' => array(
              'credentials' => '',
              'authorization' => '',
            ),
          ),
        ),
      ),
    ),
    'walkthrough-phpunit' => array(
      'operations' => array(
        'retrieve' => array(
          'enabled' => '1',
          'settings' => array(
            'services_oauth' => array(
              'credentials' => '',
              'authorization' => '',
            ),
          ),
        ),
      ),
    ),
    'walkhub-walkthrough-screening-queue' => array(
      'operations' => array(
        'index' => array(
          'enabled' => '1',
          'settings' => array(
            'services_oauth' => array(
              'credentials' => '',
              'authorization' => '',
            ),
          ),
        ),
      ),
    ),
    'walkthrough-set-phpunit' => array(
      'operations' => array(
        'retrieve' => array(
          'enabled' => '1',
          'settings' => array(
            'services_oauth' => array(
              'credentials' => '',
              'authorization' => '',
            ),
          ),
        ),
      ),
    ),
  );
  $endpoint->debug = 0;
  $export['apiv2_oauth'] = $endpoint;

  return $export;
}
