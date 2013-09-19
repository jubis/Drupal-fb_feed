<?php 

/**
*
* Custom page callback function
*  Creates the content for inside the iframe in FB Feed blocks.
*  Recognizes the block by hashed block name in url.
*
*/
function fb_feed_block_as_page() {
	$block_name_hash = arg(1);
	foreach(fb_feed_get_blocks() as $block_data) {
		if($block_name_hash == md5($block_data['name'])) {

			$theme_css = '';
			if(file_exists( drupal_get_path( 'theme', variable_get( 'theme_default',NULL ) ) .'/style.css' ) ) {
				$theme_css = drupal_get_path( 'theme', variable_get( 'theme_default',NULL ) ) .'/style.css';
			} else if( drupal_get_path( 'theme', variable_get( 'theme_default',NULL ) ) .'/css/style.css' ) {
				$theme_css = drupal_get_path( 'theme', variable_get( 'theme_default',NULL ) ) .'/css/style.css';
			}
			
			$block = array(
				'#prefix' => '<html><head>'.
				'<style type="text/css" media="all">@import url("'.theme('fb_feed_css',array()).'"); </style>'.
				'<style type="text/css" media="all">@import url("' .$theme_css. '"); </style>'.
				'</head><body>',
				'#suffix' => '</body></html>'				
			);

			$fb = fb_feed_config_api();
			
			$object = $block_data['id'];
			
			$feed_data = array();
			$object_data = array();
			try {
				$feed_data = $fb->api('/'. $object .'/feed','GET');
				$feed_data = $feed_data['data'];
				$object_data = $fb->api('/'. $object,'GET');
			} catch( FacebookApiException $e ) {
				$error = array(
					'#markup' => '<p><b>Error while loading feed and general object data from Facebook</b></p>'.
								 '<p>Be sure that the Object ID is correct '.
								 'and the Object is a page or a group.</p>'
				);
				print drupal_render($error);
				return;
			}
			
			
			switch ($block_data['type']) {
				case 'group':
					try {
						$members = $fb->api('/'. $object .'/members','GET');
						$members = $members['data'];
						
						if($block_data['header']) {
							$block['content']['head'] = fb_feed_get_group_head($object_data,$members);
						}
						
					} catch(FacebookApiException $e) {
						$block['content'] = array(
							'#markup' => '<p><b>Error while loading group '.
										 'data from Facebook</b></p>'.
										 '<p>Be sure that Object ID is correct and '.
										 'the Object really is a group.</p>'
						);
						print drupal_render($block);
						return;
					}
					break;
				case 'page':
					if($block_data['header']) {
						$block['content']['head'] = fb_feed_get_page_head($object_data);
					}

					break;
			}
			
			if( $block_data['header'] ) {
				$block['content']['head'] = array(
					'header' => $block['content']['head'],
					'#theme_wrappers' => array('html_tag_wrapper'),
					'#tag' => 'div',
					'#attributes' => array('class'=>array('fb-feed-header','clearfix'))
				);
			}
			$block['content']['scroller'] = array(
				'item_list' => fb_feed_get_post_item_list($feed_data),
				'#theme_wrappers' => array('html_tag_wrapper'),
				'#tag' => 'div',
				'#attributes' => array('class'=>array('fb-feed-scroller'))
			);
			
			print drupal_render($block);
			return;
			
		}
	}
}