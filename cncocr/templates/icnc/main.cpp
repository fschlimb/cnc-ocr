{% import "common_macros.inc.c" as util -%}
{% import "icnc/macros_icnc.inc" as util -%}

#include "{{g.name}}_context.h"

int {{g.name}}_main()
{
    // initialize data items
    {%- for output in g.initFunction.outputs %}
    {% if output.kind == 'ITEM' -%}
    {{g.itemDeclarations[output.collName].type}} {{output.binding}}_handle = /* TODO: initialization */;
    {% endif %}
    {%- endfor %}
    
    // Put initial items
    {%- for output in g.initFunction.outputs -%}
    {{util.render_put(output)}}
    {% endfor %}
}
{#/* ~k.expr 

    {%- if output.kind == 'ITEM' -%}


 */#}
