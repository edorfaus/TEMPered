var CalibrationTool = {
	create: function(container, autofocus) {
		// Only create with object containers.
		if ( typeof container != 'object' ) return;
		// Don't add it again to the same container.
		if ( container.hasCalibrationTool ) return;
		if ( /(^|\s)has-calibration(\s|$)/.test(container.className) ) return;
		container.hasCalibrationTool = true;
		container.className += ' has-calibration';
		if ( !/(^|\s)calibration(\s|$)/.test(container.className) )
		{
			container.className += ' calibration';
		}
		
		// Default value for this optional parameter.
		if ( typeof autofocus != 'boolean' ) autofocus = true;
		
		var create = CalibrationTool.createElement;
		
		var headers = create('div', {className:'calibration-headers'});
		headers.appendChild(create('label', 'Actual temperature (°C):'));
		headers.appendChild(create('label', 'Measured temperature (°C):'));
		container.appendChild(headers);
		
		for ( var i = 0 ; i < 3 ; i++ )
		{
			var valueSet = create('div', {className:'calibration-value-set'});
			valueSet.appendChild(create('input', {
				type:'number',
				className: 'calibration-value-actual',
				autofocus: (autofocus && i == 0 ),
				onchange: CalibrationTool.updateResult,
				onkeyup: CalibrationTool.updateResult,
				calibrationToolContainer: container
			}));
			valueSet.appendChild(create('input', {
				type:'number',
				className: 'calibration-value-measured',
				onchange: CalibrationTool.updateResult,
				onkeyup: CalibrationTool.updateResult,
				calibrationToolContainer: container
			}));
			container.appendChild(valueSet);
		}
		
		var resultLine = create('p', 'Calibration parameter: ');
		resultLine.appendChild( container.resultContainer = create('span') );
		container.appendChild(resultLine);
	},
	createElement: function(tag, attrs) {
		var elem = document.createElement(tag);
		if ( typeof attrs == 'string' )
		{
			elem.appendChild( document.createTextNode( attrs ) );
		}
		else if ( typeof attrs == 'object' )
		{
			for ( var attr in attrs )
			{
				elem[attr] = attrs[attr];
			}
		}
		return elem;
	},
	getValues: function(container) {
		var values = [];
		var valueSetRE = /(^|\s)calibration-value-set(\s|$)/;
		var actualRE = /(^|\s)calibration-value-actual(\s|$)/;
		var measuredRE = /(^|\s)calibration-value-measured(\s|$)/;
		var invalidRE = /(^|\s)invalid(\s|$)/;
		var isNumericRE = /^[0-9]*([.][0-9]*)?$/;
		for ( var node = container.firstChild; node; node = node.nextSibling )
		{
			if ( node.nodeType != 1 || !valueSetRE.test( node.className ) )
			{
				continue;
			}
			var valueSet = { actual: null, measured: null };
			var inputs = node.getElementsByTagName('input');
			for ( var i = inputs.length - 1; i >= 0; i-- )
			{
				var which = null;
				if ( actualRE.test( inputs[i].className ) )
				{
					which = 'actual';
				}
				else if ( measuredRE.test( inputs[i].className ) )
				{
					which = 'measured';
				}
				else
				{
					continue;
				}
				if ( valueSet[which] !== null )
				{
					valueSet = null;
					break;
				}
				valueSet[which] = inputs[i].value;
				if ( valueSet[which] === null )
				{
					valueSet[which] = '';
				}
			}
			if ( valueSet === null ||
				valueSet.actual === null ||
				valueSet.measured === null ||
				!isNumericRE.test(valueSet.actual) ||
				!isNumericRE.test(valueSet.measured)
			) {
				// Duplicate input found, or not all inputs were found,
				// or at least one of the values is invalid.
				valueSet = null;
			}
			else if ( valueSet.actual != '' && valueSet.measured != '' )
			{
				if ( valueSet.actual == '.' ) valueSet.actual = '0';
				if ( valueSet.measured == '.' ) valueSet.measured = '0';
				values.push(valueSet);
			}
			var className = node.className.replace(invalidRE, ' ');
			if ( valueSet === null )
			{
				className += ' invalid';
			}
			node.className = className;
		}
		return values;
	},
	getFactors: function(values) {
		if ( Object.prototype.toString.call(values) !== '[object Array]' )
		{
			return 'invalid values list';
		}
		if ( values.length < 1 )
		{
			return [];
		}
		if ( values.length == 1 )
		{
			return [( values[0].actual - values[0].measured )];
		}
		// T_actual = a + b * T_measured
		if ( values.length == 2 )
		{
			var Ta1 = values[0].actual;
			var Ta2 = values[1].actual;
			var Tm1 = values[0].measured;
			var Tm2 = values[1].measured;
			if ( Ta1 == Ta2 && Tm1 == Tm2 )
			{
				return CalibrationTool.getFactors( [ values[0] ] );
			}
			if ( Ta1 == Ta2 )
			{
				return 'duplicate actual temperature';
			}
			if ( Tm1 == Tm2 )
			{
				return 'duplicate measured temperature';
			}
			var b = ( Ta2 - Ta1 ) / ( Tm2 - Tm1 );
			var a = Ta1 - b * Tm1;
			return [a, b];
		}
		// T_actual = a + b * T_measured + c * T_measured * T_measured
		if ( values.length == 3 )
		{
			var Ta1 = values[0].actual;
			var Ta2 = values[1].actual;
			var Ta3 = values[2].actual;
			var Tm1 = values[0].measured;
			var Tm2 = values[1].measured;
			var Tm3 = values[2].measured;
			if ( Ta1 == Ta2 && Tm1 == Tm2 )
			{
				return CalibrationTool.getFactors( [ values[0], values[2] ] );
			}
			if ( ( Ta1 == Ta3 && Tm1 == Tm3 ) || ( Ta2 == Ta3 && Tm2 == Tm3 ) )
			{
				return CalibrationTool.getFactors( [ values[0], values[1] ] );
			}
			if ( Ta1 == Ta2 || Ta1 == Ta3 || Ta2 == Ta3 )
			{
				return 'duplicate actual temperature';
			}
			if ( Tm1 == Tm2 || Tm1 == Tm3 || Tm2 == Tm3 )
			{
				return 'duplicate measured temperature';
			}
			var Tm1_2 = Tm1 * Tm1;
			var Tm2_2 = Tm2 * Tm2;
			var Tm3_2 = Tm3 * Tm3;
			
			var Tm2_Tm1 = Tm2 - Tm1;
			
			var o = ( Ta3 - Ta1 )*Tm2_Tm1 - ( Ta2 - Ta1 )*( Tm3 - Tm1 );
			var u = ( Tm3_2 - Tm1_2 )*Tm2_Tm1 - ( Tm2_2 - Tm1_2 )*( Tm3 - Tm1 );
			
			var c = o / u;
			var b = ( Ta2 - Ta1 - c * ( Tm2_2 - Tm1_2 ) ) / Tm2_Tm1
			var a = Ta1 - b * Tm1 - c * Tm1_2
			
			return [a, b, c];
		}
		return 'too many measurements';
	},
	simplifyFactors: function(factors) {
		if ( Object.prototype.toString.call(factors) !== '[object Array]' )
		{
			return null;
		}
		while ( factors.length > 2 && factors[factors.length-1] == 0 )
		{
			factors.pop();
		}
		if ( factors.length == 2 && factors[1] == 1 )
		{
			factors.pop();
		}
		if ( factors.length == 1 && factors[0] == 0 )
		{
			factors.pop();
		}
		return factors;
	},
	updateResult: function() {
		var container = this.calibrationToolContainer;
		var result = '';
		var values = CalibrationTool.getValues(container);
		var factors = CalibrationTool.getFactors(values);
		if ( typeof factors == 'string' )
		{
			result = '[error: '+factors+']';
		}
		else if ( factors.length > 0 )
		{
			factors = CalibrationTool.simplifyFactors(factors);
			if ( factors.length == 0 )
			{
				result = '[not needed]';
			}
			else
			{
				result = '-c '+factors.join(':');
			}
		}
		var resultContainer = container.resultContainer;
		while ( resultContainer.firstChild )
		{
			resultContainer.removeChild( resultContainer.firstChild );
		}
		resultContainer.appendChild( document.createTextNode( result ) );
	}
}
