<?php
/**
 * Template file for Walkthrough Recorder Button's block.
 */
?>

<div class="row">
  <span id="record-a-walkthrough-button">
    <span class="record-action">
      <p>Record a Walkthrough</p>
      <span class="rec">REC</span>
    </span>
    <span class="save-action" style="display: none;">
      <p>Save your recorded Walkthrough</p>
      <i class="fa fa-floppy-o"></i>
    </span>
  </span>
  <div id="record-a-walkthrough-popup-modal">
    <div id="record-a-walkthrough-popup" class="recorder-popup small-12 large-4 large-centered columns">
      <div class="row" id="walkthrough-record-popup-title">
        <p class="small-10 columns walkthrough-record-first-step-title">Where would you like to record your Walkthrough?</p>
        <p class="small-10 columns walkthrough-record-second-step-title" style="display: none;">Save your Walkthrough!</p>
        <i class="fa fa-times close-popup"></i>
      </div>
      <div class="row" id="walkthrough-record-first-step">
        <div class="small-10 columns">
          <input type="text" id="walkthrough-record-url" name="walkthrough-url" value="">
          <div class="walkthrough-record-form-error" id="url-error" style="display: none;">This doesn't seem to be a valid URL.<br>Required format: http://example.com</div>
        </div>
        <div class="small-2 columns">
          <span id="walkthrough-start-recording">REC</span>
        </div>
      </div>
      <div class="row" id="walkthrough-record-second-step" style="display: none;">
        <div class="small-12 columns" id="walkthrough-record-title-wrapper">
          <div class="small-2 columns">
            <label for="" class="left inline">Title</label>
          </div>
          <div class="small-10 columns form-element"></div>
          <div class="small-12 columns walkthrough-record-form-error" id="walkthrough-record-title-error" style="display: none;">Please enter a title for your Walkthrough.</div>
        </div>
        <div class="small-12 columns" id="walkthrough-record-severity-wrapper">
          <div class="small-5 columns">
            <label for="" class="left inline">Severity</label>
          </div>
          <div class="small-7 columns form-element"></div>
          <div class="small-12 columns walkthrough-record-form-error" id="walkthrough-record-severity-error" style="display: none;">Walkthroughs interact with your site, if played automatically they can break it. Mark your Walkthrough appropriately to prevent problems.</div>
        </div>
        <div class="small-12 columns">
          <div class="small-12 columns">
            <label class="left inline">Recorded Steps</label>
          </div>
          <div class="small-12 columns">
            <textarea id="walkthrough-recorded-steps" readonly=""></textarea>
          </div>
        </div>
        <div class="small-12 columns walkthrough-record-popup-action-button-wrapper">
          <div class="small-4 columns">
            <button id="walkthrough-record-save"><i class="fa fa-floppy-o"></i> Save</button>
          </div>
          <div class="small-8 columns">
            <button id="walkthrough-record-reset"><i class="fa fa-file-o"></i> Delete, and start a new recording</button>
          </div>
        </div>
      </div>
      <div class="row" id="walkthrough-recorder-popup-notice">
        <p class="small-12 columns">You are about to record a Walkthrough. The recorder is still in alpha, we need your help to make it better. If something goes wrong, let us know so that we can fix it (send a mail to <a href="mailto:kata@pronovix.com?subject=Walkthrough recorder error">kata@pronovix.com</a>).</p>
      </div>
    </div>
  </div>
  <div id="record-a-walkthrough-hidden-form" style="display: none;"><?php echo drupal_render($form); ?></div>
</div>
