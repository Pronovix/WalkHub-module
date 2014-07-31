<span class="icon-unlock"></span>
<div class="switch tiny">
  <input id="switch-private" name="walkthrough-private-public" type="radio" onclick="window.location.href='/node/<?php echo $nid; ?>/edit';" <?php echo ($status ? '' : 'checked="checked"'); ?>>
  <label for="switch-private" onclick=""></label>
  <input id="switch-public" name="walkthrough-private-public" type="radio" onclick="window.location.href='/node/<?php echo $nid; ?>/edit';" <?php echo ($status ? 'checked="checked"' : ''); ?>>
  <label for="switch-public" onclick=""></label>
  <span></span>
</div>
<span class="icon-lock"></span>
<div class="walkthrough-status-text">This walkthrough is <strong><?php echo ($status ? 'public' : 'private'); ?></strong></div>
