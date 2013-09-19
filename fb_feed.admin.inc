<?php
/**
*
* Callback function for menu item admin/config/content/fb_feed 
* Functions as module configuration page
*
* @see fb_feed_menu()
*/
function fb_feed_form($form,&$form_state) {
	$form['fb_feed_app_id'] = array(
		'#type' => 'textfield',
		'#title' => t("App ID"),
		'#default_value' => variable_get('fb_feed_app_id',''),
		'#size' => 60,
		'#maxlength' => 60,
		'#description' => t('App ID of the Facebook App'),
		'#required' => TRUE
	);
	
	$form['fb_feed_secret'] = array(
		'#type' => 'textfield',
		'#title' => t("App Secret"),
		'#default_value' => variable_get('fb_feed_secret',''),
		'#size' => 60,
		'#maxlength' => 60,
		'#description' => t('App secret of Facebook App'),
		'#required' => TRUE
	);
	
	return system_settings_form($form);
}

/**
*
* Implements form Api form_validate
*   Checks if Facebook API call can be made with given values
*
*/
function fb_feed_form_validate($form,&$form_state) {
	fb_feed_require_api();

	$app_id = $form_state['values']['fb_feed_app_id'];
	$secret = $form_state['values']['fb_feed_secret'];
	
	$config = array();
	$config['appId'] = $app_id;
	$config['secret'] = $secret;
	$fb = new Facebook($config);
	
	try{
		$fb->api('/platform','GET');
	} catch(FacebookApiException $e) {
		form_set_error('fb_feed_app_id',t('App ID may not be valid'));
		form_set_error('fb_feed_secret',t('Secret may not be valid'));
	}
	
}

/**
*
* Callback function for menu item admin/structure/fb_feed
* Block creator/editor
*
*/
function fb_feed_block_editor($form,$form_state,$p_block_name = '') {
	//var_dump($form_state);
	//var_dump($form);
	$edit = FALSE;
	$form_name = 'new';
	$submit = 'Create';
	$fieldset_title = 'Create new';
	$block = array('name'=>'','title'=>'','type'=>'','id'=>'','header'=>TRUE);
	
	$in_block_config = FALSE;
	$block_name = arg(3);
	if($p_block_name != '') {
		$in_block_config = TRUE;
		$block_name = $p_block_name;
		$edit = TRUE;
	}
	$correct_block_name = FALSE;
	if(arg(4) == 'edit' || $edit) {
		$blocks = db_select('fb_feed_blocks','b')
					->fields('b',array('name'))
					->execute()
					->fetchAllAssoc('name',PDO::FETCH_ASSOC);
		//var_dump($blocks);
		foreach($blocks as $b) {
			if($b['name'] == $block_name) {
				$edit = TRUE;
				$form_name = 'edit';
				$submit = 'Save';
				$fieldset_title = 'Edit';
				$correct_block_name = TRUE;
	
				$block = db_select('fb_feed_blocks','b')
							->condition('name',$block_name,'=')
							->fields('b')
							->execute()
							->fetchAssoc();
				//var_dump($block);
			}
		}
		
	} 
	
	if(!$correct_block_name && arg(4) == 'edit') {
		$form['fb_feed_edit_block'] = array(
			'#markup' => t('Block <b>'.$block_name."</b> doesn't exist. You can ". l('create new block','admin/structure/fb_feed') .' instead.') 
		);
	}
	else {
		$form['fb_feed_'.$form_name.'_block'] = array(
			'#type' => 'fieldset',
			'#title' => t($fieldset_title.' block'),
			'#collapsible' => TRUE,
			'#collapsed' => TRUE,
			'edit' => array(
				'#type' => 'hidden',
				'#value' => $edit
			),
			'feed_title' => array(
				'#type' => 'textfield',
				'#title' => t('Block title'),
				'#description' => t('Default title for block. This still can be overriden in block configuration.<br>Title will be used in block name in admin/structure/block.'),
				'#required' => TRUE,
				'#default_value' => $block['title']
			),
			'name'=> array(
				'#type' => 'textfield',
				'#title' => t('Machine readable name'),
				'#description' => t('Module uses machine readable name as identifier for this block. Also used e.g. in URL paths.<br>Use only a-z and _.'),
				'#required' => TRUE,
				'#default_value' => $block['name']
			),
			'type' => array(
				'#type' => 'select',
				'#title' => t('Feed type'),
				'#description' => t("Select which kind of Facabook object you want to get feed from."),
				'#options' => array('group' => t('Facebook Group'), 'page' => t('Facebook Page')),
				'#required' => TRUE,
				'#default_value' => $block['type']
			),
			'object-id' => array(
				'#type' => 'textfield',
				'#title' => t('Object ID'),
				'#description' => t('Long integer or text. Find this ID in the url of the Facebook page of the desired object.<br>e.g. www.facebook.com/groups/<b>330028033714601</b> or www.facebook.com/<b>platform</b>'),
				'#required' => TRUE,
				'#default_value' => $block['id']
			),
			'header' => array(
				'#type' => 'checkbox',
				'#title' => t('View header in block'),
				'#description' => t('View header on top of the block. Header contains some information about the object e.g. name as a link to it.'),
				'#default_value' => $block['header']
			),
			'save' => array(
				'#type' => 'submit',
				'#value' => t($submit.' block')
			),
			'to_block_list' => array(
				'#markup' => l( t('To FB Feed block list'), 'admin/structure/fb_feed' )
			)
		);
	}
	if($edit) {
		$form['fb_feed_'.$form_name.'_block']['name']['#disabled'] = TRUE; 
		$form['fb_feed_'.$form_name.'_block']['#collapsible'] = FALSE;
		//$form['fb_feed_'.$form_name.'_block']['#collapsed'] = FALSE;
	}
	if($in_block_config) {
		$form['fb_feed_'.$form_name.'_block']['save'] = array();
	}
	if( ! $edit ) {
		$form['links'] = fb_feed_links_to_block_editors();
		$form['fb_feed_'.$form_name.'_block']['to_block_list'] = array();
	}
	
	return $form;
}

/**
*
* Implements form_validate for block editor
*
*/
function fb_feed_block_editor_validate($form,$form_state,$edit = array()) {
	if( ! empty( $form_state ) ) {
		$val = $form_state['values'];
	}
	else {
		return;
	}
	if( ! $val['edit'] ) {
		$blocks = fb_feed_get_blocks( array( 'name' ) );
		foreach( $blocks as $block ) {
			if( $block['name'] == $val['name'] ) {
				form_set_error( 'name', 'The machine readable name <i>'. $val['name'] .'</i> is already in use.' );
			}
		}
	}
	
	
	
}

/**
*
* Implements form_submit for block editor
*
*/
function fb_feed_block_editor_submit($form,$form_state,$edit = array()) {
	$val = array();
	if(empty($form_state)) {
		$val = $edit;
	}
	else {
		$val = $form_state['values'];
	}
	
	
	
	if(!$val['edit']) {
		db_insert('fb_feed_blocks')
		->fields(array(
			'name' => $val['name'],
			'title' => $val['feed_title'],
			'type' => $val['type'],
			'id' => $val['object-id'],
			'header' => $val['header']
		))
		->execute();
		drupal_goto('admin/structure/fb_feed');
		drupal_set_message('Block <b>'.$val['name'].'</b> was created');
	}
	else {
		db_update('fb_feed_blocks')
		->condition('name',$val['name'],'=')
		->fields(array(
			'name' => $val['name'],
			'title' => $val['feed_title'],
			'type' => $val['type'],
			'id' => $val['object-id'],
			'header' => $val['header']
		))
		->execute();
		drupal_set_message('Block <b>'.$val['name'].'</b> was saved');
	}
}

function fb_feed_links_to_block_editors() {
	$links = array();
	
	foreach( fb_feed_get_blocks(array('name','title')) as $i => $block ) {
		$links[$i] = l( $block['title'], 'admin/structure/fb_feed/' . $block['name'] . '/edit' );
		$links[$i] .= ' - ';
		$links[$i] .= l( 'Edit', 'admin/structure/fb_feed/' . $block['name'] . '/edit' );
		$links[$i] .= ' - ';
		$links[$i] .= l( 'Delete', 'admin/structure/fb_feed/' . $block['name'] . '/delete' );
	}
	$link_list = array(
		'#type' => 'fieldset',
		'#title' => 'All blocks',
		'list' => array(
			'#theme' => 'item_list',
			'#items' => $links
		)
	);
	
	return $link_list;
}

/**
*
* Custom callback function for delete form
*
*/
function fb_feed_block_delete( $form, $form_state ) {
	$destination = $_SERVER['HTTP_REFERER'];
	if($destination == '') {
		$destination = 'admin/structure/fb_feed';
	}
	
	$block_name = arg(3);
	$blocks = fb_feed_get_blocks();
	$block_data = array();
	foreach( $blocks as $block ) {
		if( $block['name'] == $block_name ) {
			$block_data = $block;
		}
	}
	if( empty($block_data) ) {
		echo('Empty block');
		$form['fb_feed_delete_block'] = array(
			'#markup' => 'Incorrect block name <i>'. $block_name .'</i>. Find blocks in '. l( 'block list', 'admin/structure/fb_feed/' ) .'.'
		);
		return $form;
	}

	$form['fb_feed_delete_block'] = array(
		'#type' => 'fieldset',
		'#title' => 'Delete block <i>'. $block_data['name'] .'</i>',
		'delete_warning' => array( 
			'#markup' => '<p><b>Do you really want to delete <i>'. $block_data['name'] .'</i>?</b></p>'.
						 '<p>The block will be removed permanently from database.</p>'.
						 '<p>If you just don\'t want to show it for the users '.
						 'modify block visibility settings in '.
						 l('Block configuration', 'admin/structure/block/manage/fb_feed/'. $block_data['name'] .'/configure' ).'.</p>'
		),
		'confirm_button' => array(
			'#type' => 'submit',
			'#value' => 'Confirm'
		),
		'cancel_deleting' => array(
			'#markup' => l( 'Cancel', $destination )
		),
		'block_name' => array(
			'#type' => 'hidden',
			'#value' => $block_data['name']
		)
	);
	//$form['#redirect'] = $destination;
	var_dump( $form );
	
	return $form;
}

function fb_feed_block_delete_validate( $form, $form_state ) {
	$values = $form_state['values'];
	
	$block_name = $values['block_name'];
	$blocks = fb_feed_get_blocks( array('name') );
	$block_data = array();
	foreach( $blocks as $block ) {
		if( $block['name'] == $block_name ) {
			$block_data = $block;
		}
	}
	if( empty($block_data) ) {
		form_set_error( 'block_name', 'Trying to delete non-existent block <i>'. $values['block_name'] .'</i>' );
	}
}

function fb_feed_block_delete_submit( $form, $form_state ) {
	$values = $form_state['values'];
	//var_dump($values);
	
	fb_feed_delete_block( $values[ 'block_name' ] );
	
	drupal_set_message( 'Block <i>'. $values['block_name'] .'</i> has been deleted.' );
	drupal_goto( 'admin/structure/fb_feed' );
}