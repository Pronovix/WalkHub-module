<div class="walkthrough-step">
  <span class="walkthrough-step-title"><?php print check_plain($node->title); ?></span>

  <?php if(!empty($node->content['body'][0]['#markup'])): ?>
    <div class="walkthrough-step-body"><?php print $node->content['body'][0]['#markup']; ?></div>
  <?php endif; ?>

  <span>(<a class="walkthrough-edit" href="<?php print url("node/{$node->nid}/edit"); ?>">Edit</a>)</span>
</div>
