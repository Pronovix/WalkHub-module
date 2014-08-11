<span class="icon-unlock"></span>
<div class="switch tiny">
  <input id="switch-private" name="walkthrough-private-public" type="radio" value="" data-token="<?php echo $token; ?>" data-nid="<?php echo $nid; ?>" <?php echo ($status ? '' : 'checked="checked"'); ?>>
  <label for="switch-private"></label>
  <input id="switch-public" name="walkthrough-private-public" type="radio" value="1" data-token="<?php echo $token; ?>" data-nid="<?php echo $nid; ?>" <?php echo ($status ? 'checked="checked"' : ''); ?>>
  <label for="switch-public"></label>
  <span></span>
</div>
<span class="icon-lock"></span>
<div class="walkthrough-status-text">This walkthrough is <strong><?php echo ($status ? 'public' : 'private'); ?></strong></div>
