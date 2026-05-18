module TypeSanitizer
  def self.sanitize_c_identifier(unsanitized)
    # преобразовать имя файла в действительный идентификатор C, заменив недопустимые символы на '_'
    unsanitized.gsub(/[-\/\\.,\s]/, '_')
  end
end
