def validate_required(fields: dict):
    for key, value in fields.items():
        if str(value).strip() == "":
            return False, f"{key} is required"
    return True, "OK"
